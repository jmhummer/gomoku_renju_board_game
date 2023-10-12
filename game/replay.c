#include "game.h"
#include "board.h"
#include "io.h"
#include "error-codes.h"
#include <string.h>

/**
 * Prints out the error message to the console if command line args are not correct.
 */
static void arg_error();


/**
 * Replays a given game from a saved file.
 * Displays each move with a list of moves so far.
 * Plays one move per second.
 * @param argc The total number of arguments.
 * @param argv[] Array of arguments
 * return 0 if successful.
 */
int main( int argc, char *argv[] ) {
    
    if ( argc != 2 ) {
        arg_error();
    }
    
    char* path = argv[1];
    game* g = game_import( path );
    game_replay( g );
    
    return 0;
}

static void arg_error() {
    printf( "usage: ./replay <saved-match.gmk>\n" );
    exit( ARGUMENT_ERR );
}