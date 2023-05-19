/* client.c    Kyrylo Bakumenko    20 May, 2023
 *
 * This file ...
 * 
 * Exit codes: 1 -> invalid number of arguments
 *             2 -> null argument passed
 *             3 -> failure to initialize messages module
 *             4 -> bad hostname or port
 *             5 -> 
 */

#include <stdlib.h>
#include "../support/message.h"

// internal function prototypes

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

    // /* assign command-line arguments to variables */
    // // commandline provides address for server
    // const char* serverHost = argv[1];
    // const char* serverPort = argv[2];
    // server; // address of the server
    // if (!message_setAddr(serverHost, serverPort, &server)) {
    //     fprintf(stderr, "ERROR: Failure forming address from %s %s\n", serverHost, serverPort);
    //     exit(4); // bad hostname/port
    // }

    // shut down the message module
    message_done();

    return 0;
}
