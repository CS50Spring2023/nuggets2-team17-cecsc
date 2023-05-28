/* client.c    Kyrylo Bakumenko    20 May, 2023
 *
 * This file ...
 * 
 * Exit codes: 1 -> invalid number of arguments
 *             2 -> null argument passed
 *             3 -> failure to initialize messages module
 *             4 -> bad hostname or port
 *             5 -> ioctl error
 *             6 -> failed screen verification
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ncurses.h>
#include "message.h"

// internal function prototypes
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
static void display_map(char* display);
static void initialize_curses();      // CURSES


// global variables
const char* valid_message_headers[] = {"OK", "GRID", "GOLD", "DISPLAY", "QUIT", "ERROR"};
const int num_headers = 6;

// size of board, initialized to initial window size
static int NROWS;
static int NCOLS;

// TEMP FOR DEBUGGING
char* MAP = "  +----------+  |..........|  |....A.....|  +-#--------+    #             #             #           +-#--------+  |..........|  |..........|  +----------+";

/* ***************************
 *  main function
 *  Accepts either 2 or 3 arguments from command-line: hostname port [playername]
 *  
 */
int main(int argc, char *argv[])
{
    /* parse the command line, validate parameters */ 
    const char* program = argv[0];
    // check num parameters
    if (argc != 3 && argc != 4) {
        fprintf(stderr, "ERROR: Expected either 2 or 3 arguments but recieved %d\n", argc-1);
        fprintf(stderr, "USAGE: %s hostname port [playername]\n", program);
        exit(1); // bad num arguments
    }
    // Defensive programming, check for null arguments
    for (int i = 1; i < argc; i++) {
        if (argv[i] == NULL) {
            fprintf(stderr, "ERROR: argmument %d is NULL\n", i);
            exit(2); // bad argument
        }
    }

    /* initialize messages module */
    // (without logging)
    if (message_init(NULL) == 0) {
        exit(3); // failure to initialize message module
    }

    /* assign command-line arguments to variables */
    // commandline provides address for server
    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    addr_t server; // address of the server
    if (!message_setAddr(serverHost, serverPort, &server)) {
        fprintf(stderr, "ERROR: Failure forming address from %s %s\n", serverHost, serverPort);
        exit(4); // bad hostname/port
    }

    // Loop, waiting for input or for messages; provide callback functions.
    // We use the 'arg' parameter to carry a pointer to 'server'.
    
    bool ok = message_loop(&server, 0, NULL, handleInput, handleMessage);

    // shut down the message module
    message_done();

    return ok ? 0 : 1;
}

/**************** handleInput ****************/
/* stdin has input ready; read a line and send it to the client.
 * Return true if the message loop should exit, otherwise false.
 * i.e., return true if EOF was encountered on stdin, or fatal error.
 */
static bool
handleInput(void* arg)
{
  // We use 'arg' to receive an addr_t referring to the 'server' correspondent.
  // Defensive checks ensure it is not NULL pointer or non-address value.
  addr_t* serverp = arg;
  if (serverp == NULL) {
    fprintf(stderr, "handleInput called with arg=NULL");
    return true;
  }
  if (!message_isAddr(*serverp)) {
    fprintf(stderr, "handleInput called without a correspondent.");
    return true;
  }
  
  // allocate a buffer into which we can read a line of input
  // (it can't be any longer than a message)!
  char line[message_MaxBytes];

  // read a line from stdin
  if (fgets(line, message_MaxBytes, stdin) == NULL) {
    // EOF case: stop looping
    return true;
  } else {
    // strip trailing newline
    const int len = strlen(line);
    if (len > 0 && line[len-1] == '\n') {
      line[len-1] = '\0';
    }

    // send as message to server
    message_send(*serverp, line);

    // normal case: keep looping
    return false;
  }
}

/**************** handleMessage ****************/
/* Datagram received; print it.
 * We ignore 'arg' here.
 * Return true if any fatal error.
 */
static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
  // defensive programming
  if (arg == NULL || message == NULL) {
    return false;
  }

  // Find the position of the header within the message
  if (strstr(message, "OK") != NULL){
    /* initialize curses library */
    initialize_curses(); // CURSES
    display_map("");
    // CONFIRMATION??
    message_send(from, "OK");

  } else if (strstr(message, "GRID") != NULL) {
    // assert the size of the grid
    // char* msg_cpy = malloc(strlen(message)+1);
    char* nrows_c;
    char* ncols_c;
    // strcpy(msg_cpy, message);
    strtok((char*)message, " ");
    // assign values and check for null
    if ( (nrows_c = strtok(NULL, " ")) == NULL || (ncols_c = strtok(NULL, " ")) == NULL) {
      message_send(from, "BAD");
      return false;
    }
    // if not null, verify screen size
    int nrows;
    int ncols;
    sscanf(nrows_c, "%d", &nrows);
    sscanf(ncols_c, "%d", &ncols);
    if (nrows > NROWS || ncols > NCOLS) {
      fprintf(stderr, "ERROR: incompatible screen size for [%d, %d]\n", nrows, ncols);
      exit(6);
    } else {
      NROWS = nrows;
      NCOLS = ncols;
    }
    // CONFIRMATION??
    message_send(from, "OK");

  } else if (strstr(message, "GOLD") != NULL) {
    // character dummies to rea from message
    char* n;
    char* p;
    char* r;
    // ints to store gold information to display in curse
    int gold_collected;
    int gold_purse;
    int gold_left;
    strtok((char*)message, " ");
    if ((n = strtok(NULL, " ")) == NULL || (p = strtok(NULL, " ")) == NULL || (r = strtok(NULL, " ")) == NULL ) {
      message_send(from, "BAD");
      // INVALID GOLD MESSAGE SENT
    } else {
      // copy char value into int
      sscanf(n, "%d", &gold_collected);
      sscanf(p, "%d", &gold_purse);
      sscanf(r, "%d", &gold_left);
      // display info message
      mvprintw(0,0, "Gold Collected: %d\tGold in Purse: %d\tGold Left: %d", gold_collected, gold_purse, gold_left);    // CURSES
      refresh();                              // CURSES
    }
    // CONFIRMATION??
    message_send(from, "OK");

  } else if (strstr(message, "DISPLAY") != NULL) {
    /* get display contents */
    size_t headerLength = strlen("DISPLAY\n") + 1;
    size_t displayLength = strlen(message) - headerLength + 1;
    // display buffer
    char display[displayLength]; 
    strncpy(display, message + headerLength, displayLength);
    display[displayLength] = '\0'; // in-case length is such that last terminating character is not copied
    
    // print display
    display_map(display);

    // CONFIRMATION??
    message_send(from, "OK");
    return false;

  } else if (strstr(message, "QUIT") != NULL) {
    fprintf(stdout, "%s", message);
    return true;

  } else if (strstr(message, "ERROR") != NULL) {

  } else {
    message_send(from, "No valid header present!\n");
    fprintf(stderr, "No valid header present!\n");
    return false;
  }
  
  return false;
}

/* ************ initialize_curses *********************** */
/*
 * initialize curses // CURSES everywhere in this function
 */
static void
initialize_curses()
{
  // initialize the screen - which defines LINES and COLS
  initscr();

  // cache the size of the window in our global variables
  // (this is a macro, which is why we don't need & before the variables.)
  getmaxyx(stdscr, NROWS, NCOLS);

  cbreak(); // actually, this is the default
  // noecho(); // don't show the characters users type

  // I like yellow on a black background
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  fprintf(stdout, "~~INITIALIZED~~\n");
  fflush(stdout);
}

/* ************ display_map *********************** */
/*
 * Display the map onto the screen;
 */
static void
display_map(char* display)
{
  for (int row = 0; row < NROWS; row++) {
    for (int col = 0; col < NCOLS; col++) {
      move(row+1,col);    // CURSES, +1 account for info line
      int idx = row * NCOLS + col;
      if (idx < strlen(display)) {                
        addch(display[row * NCOLS + col]);     // CURSES
      } else {
        addch(' ');               // fill with blank
      }
    }
  }
  refresh();                              // CURSES
}

// static char*
// message_header(const char* message)
// {
//   char* header;
//   char* msg_cpy = malloc(strlen(message)+1);
//   strcpy(msg_cpy, message);
//   // defensive programming
//   if ((header = strtok(msg_cpy, " ")) == NULL) {
//     return NULL;
//   }
//   for (int i = 0; i < num_headers; i++) {
//     // fprintf(stdout, "%s\n", valid_message_headers[i]);
//     if (strcmp(header, valid_message_headers[i]) == 0) {
//       return header;
//     }
//   }
//   free(msg_cpy);

//   return NULL;
// }