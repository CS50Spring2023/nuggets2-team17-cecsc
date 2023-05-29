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
static int NROWS = -1;
static int NCOLS = -1;

// TEMP FOR DEBUGGING
char* MAP = "  +----------+  |..........|  |.A........|  +-#--------+    #             #             #           +-#--------+  |..........|  |..........|  +----------+";

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
    endwin(); // CURSES

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

  // if curses uninitialized
  if (NROWS == -1 || NCOLS == -1) {
  
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
  } else {
    // gather key input
    char c = getch();
    char message[strlen("KEY " + 2)];
    strcpy(message, "KEY ");
    strcat(message, &c);

    // send keystroke
    message_send(*serverp, message);

    // keep looping
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
  if (strncmp(message, "OK ", strlen("OK ")) == 0){
    /* initialize curses library */
    initialize_curses(); // CURSES
    display_map("");
    // CONFIRMATION??
    message_send(from, "OK");

  } else if (strncmp(message, "GRID ", strlen("GRID ")) == 0) {
    int nrows;
    int ncols;
    // assign values and check for null
    if (sscanf(message, "GRID %d %d", &nrows, &ncols) != 2) {
      fprintf(stderr, "ERROR: Malformed GRID message '%s'", message);
      return false;
    }
    // if not null, verify screen size
    if (nrows > NROWS || ncols > NCOLS) {
      endwin(); // CURSES
      fprintf(stderr, "ERROR: incompatible screen of size [%d, %d] for [%d, %d]\n", NROWS, NCOLS, nrows, ncols);
      exit(6);
    } else {
      // update with screen output dimensions
      NROWS = nrows;
      NCOLS = ncols;
    }
    // CONFIRMATION??
    message_send(from, "OK");

  } else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0) {
    // ints to store gold information to display in curse
    int gold_collected;
    int gold_purse;
    int gold_left;
    if (sscanf(message, "GOLD %d %d %d", &gold_collected, &gold_purse, &gold_left) != 3) {
      // INVALID GOLD MESSAGE SENT
      fprintf(stderr, "ERROR: Malformed GOLD message '%s'", message);
      // CONFIRMATION??
      message_send(from, "IGNORED");
    } else {
      // display info message
      for (int col = 0; col < NCOLS; col++) {
        move(0, col);
        addch(' ');
      }
      mvprintw(0,0, "Gold Collected: %d\tGold in Purse: %d\tGold Left: %d", gold_collected, gold_purse, gold_left);    // CURSES
      refresh();                              // CURSES
      // CONFIRMATION??
      message_send(from, "OK");
    }

  } else if (strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
    /* get display contents */
    size_t headerLength = strlen("DISPLAY\n");
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

  } else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
    fprintf(stdout, "%s\n", message);
    return true;

  } else if (strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
    fprintf(stdout, "ERROR: From server '%s'\n", message);

  } else {
    // fprintf(stderr, "ERROR: Malformed message '%s'\n", message);
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
  noecho(); // don't show the characters users type

  // I like yellow on a black background
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  attron(COLOR_PAIR(1));
  fprintf(stdout, "INITIALIZED CURSES\n");
  fflush(stdout);
}

/* ************ display_map *********************** */
/*
 * Display the map onto the screen;
 */
static void
display_map(char* display)
{
  // clear output
  int max_nrows;
  int max_ncols;
  getmaxyx(stdscr, max_nrows, max_ncols);
  for (int row = 0; row < max_nrows; row++) {
    for (int col = 0; col < max_ncols; col++) {
      addch(' ');               // fill with blank
    }
  }
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
