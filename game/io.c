#include "io.h"
#include "error-codes.h"
#include "board.h"
#include "game.h"
#include <stdio.h>


game* game_import(const char* path) 
{
    FILE *file = fopen( path, "r" );
    
    if ( file == NULL ) {
        exit ( FILE_INPUT_ERR );
    }
    
    //First line should be GA
    int G = fgetc( file );
    int A = fgetc( file );
    if ( G != 'G' || A != 'A' ) {
        exit( FILE_INPUT_ERR );
    }
    
    //Line 2: Board size
    unsigned char board_size;
    fscanf( file, " %hhu", &board_size );
    
    //Line 3: Game type
    unsigned char type;
    fscanf( file, " %hhu", &type );
    //Create game from above information
    game* g = game_create( board_size, type );
    
    //Line 4: game state
    unsigned char state;
    fscanf( file, " %hhu", &state );
    //Bounds check state
    if ( state < GAME_STATE_FORBIDDEN || state > GAME_STATE_FINISHED ) {
            exit( FILE_INPUT_ERR );
    }
    g->state = state;
    
    //Line 5: Winner
    unsigned char winner;
    fscanf( file, " %hhu", &winner );
    //Bounds check winner
    if ( winner < EMPTY_INTERSECTION || winner > WHITE_STONE ) {
        exit( FILE_INPUT_ERR );
    }
    g->winner = winner;
    
    //Go through remaining lines and place stones accordingly
    unsigned char x = 0;
    unsigned char y = 0;
    char* formal_coord = (char*)malloc( 3 * sizeof( char ) );
    int scanner = fscanf( file, " %s", formal_coord );
    unsigned char convert_success = SUCCESS;
    //bool stone_placed = true;
    while ( convert_success == SUCCESS ) {
        convert_success = board_coord( g->board, formal_coord, &x, &y );
        
       // if ( g->state != GAME_STATE_PLAYING && g ->state != GAME_STATE_STOPPED ) {
       //     break;
       // }
        
        //Place stone and save
        if ( convert_success == SUCCESS ) {
            board_set( g->board, x, y, g->stone );
            save_move( g, x, y );
        } 
        
        //Switch players
        if ( g->stone == BLACK_STONE ) {
            g->stone = WHITE_STONE;
        } else {
            g->stone = BLACK_STONE;
        }

        scanner = fscanf( file, " %s", formal_coord );
        
        //Check for EOF
        if ( scanner == EOF ) {
            break;
        }
    }
    
    fclose( file );
    return g;
}

void game_export(game* g, const char* path) 
{
    FILE *file = fopen( path, "w" );
    
    //First line should be GA
    fputc( 'G', file );
    fputc( 'A', file );
    fputc( '\n', file );
    
    //Line 2: Board size
    fprintf( file, "%hhu", g->board->size );
    fputc( '\n', file );
    
    //Line 3: Game type
    fprintf( file, "%hhu", g->type );
    fputc( '\n', file );

    //Line 4: game state
    fprintf( file, "%hhu", g->state );
    fputc( '\n', file );
    
    //Line 5: Winner
    fprintf( file, "%hhu", g->winner );
    fputc( '\n', file );
    
    //Go through remaining lines and place stones accordingly
    unsigned char num_moves = ( g->moves_count / sizeof( move ) );
    char* formal_coord = (char*)malloc( 4 * sizeof( char ) );
    formal_coord[3] = '\0'; //set null terminator
    unsigned char x;
    unsigned char y;
    unsigned char convert_success;
    unsigned char write_success;
    for ( int i = 0; i < num_moves; i++ ) {
        x = g->moves[i].x;
        y = g->moves[i].y;
        convert_success = board_formal_coord( g->board, x, y, formal_coord );
        
        if ( convert_success != SUCCESS ) {
            exit ( FORMAL_COORDINATE_ERR );
        }
        
        write_success = fprintf( file, "%s\n", formal_coord );
        
        if ( write_success < 0 ) {
            exit ( FILE_OUTPUT_ERR );
        }
    }
    
    free( formal_coord );
    fclose( file );
}
    