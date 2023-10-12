#include "game.h"
#include "error-codes.h"
#include "board.h"





/**
 * Checks if the area on the given board surrounding a given coordinate and finds the longest continuous 
 * line of stones it connects to. Also determines the number of open fours surrounding the given location.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param open_fours A counter that is incremented each time an open four is found at the given coordinates.
 */
static unsigned char find_max_line( const game* g, const unsigned char x, const unsigned char y, unsigned char* open_fours );

/**
 * Determines the number of stones above the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_north( const game* g, unsigned char x, unsigned char y, unsigned char* pos );

/**
 * Determines the number of stones below the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_south( const game* g, unsigned char x, unsigned char y, unsigned char* neg );

/**
 * Determines the number of stones left of the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_east( const game* g, unsigned char x, unsigned char y, unsigned char* pos );

/**
 * Determines the number of stones right of the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_west( const game* g, unsigned char x, unsigned char y, unsigned char* pos );

/**
 * Determines the number of stones above and right of the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_northeast( const game* g, unsigned char x, unsigned char y, unsigned char* pos );

/**
 * Determines the number of stones below and left the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_southeast( const game* g, unsigned char x, unsigned char y, unsigned char* neg );

/**
 * Determines the number of stones above and left of the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_northwest( const game* g, unsigned char x, unsigned char y, unsigned char* pos );

/**
 * Determines the number of stones below and left of the given coordinates that are the same color as the current stone.
 * Also determines if the last stone of a matching color is followed by an empty intersection.
 * @param g The game to check.
 * @param x The horizontal coordinate of the last stone placed.
 * @param y The vertical coordinate of the last stone placed.
 * @param pos External counter of the number of matching stones found.
 * @return True if the last matching stone is followed by an empty intersection, false otherwise.
 */
static bool count_southwest( const game* g, unsigned char x, unsigned char y, unsigned char* pos );

/**
 * Determines if the given coordinates are within the boundaries of the board.
 * @param g The game to check.
 * @param x The horizontal coordinate of the location to check.
 * @param y The vertical coordinate of the location to check.
 * @return True if the coordinates are on the board, false otherwise.
 */
static bool in_bounds( const game* g, unsigned char x, unsigned char y );

game* game_create(unsigned char board_size, unsigned char game_type) 
{
    game *g = ( game *)malloc( sizeof( game ) );
    if (g == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory\n");
        exit(1);
    }
    g->board = board_create( board_size );
    
    //Bounds check type and assign
    if ( game_type != GAME_FREESTYLE && game_type != GAME_RENJU ) {
        exit ( INPUT_ERR );
    }
    g->type = game_type;
    
    
    g->stone = BLACK_STONE;
    g->state = GAME_STATE_PLAYING;
    g->winner = EMPTY_INTERSECTION;
    g->moves = ( move * )malloc( INITIAL_CAPACITY * sizeof( move ) );
    if (g->moves == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory\n");
        exit(1);
    }
    g->moves_count = 0;
    g->moves_capacity = INITIAL_CAPACITY;
    return g;
}

void game_delete(game* g) {
    if (g == NULL ) {
        exit( NULL_POINTER_ERR );
    }
    board_delete( g->board );
    free( g->moves );
    free( g );
}

bool game_update(game* g) 
{
    bool convert_success = false;
    unsigned char* x = ( unsigned char* )malloc( sizeof( unsigned char ) );
    unsigned char* y = ( unsigned char* )malloc( sizeof( unsigned char ) );
    char* formal_coord = ( char* )malloc( 2 * sizeof( char ) );
    bool stone_placed = false;
    do {
        //Print message
        if ( g->stone == BLACK_STONE ) {
            printf( "Black stone's turn, please enter a move: " );
        } else if ( g->stone == WHITE_STONE ) {
            printf( "White stone's turn, please enter a move: " );
        } else if ( g->stone != BLACK_STONE && g->stone != WHITE_STONE ) {
            exit( STONE_TYPE_ERR );
        }
        
        
            //Scan input and check for EOF
            int scan;
            scan = scanf( " %s", formal_coord );
            
            if ( scan == EOF ) {
                g->state = GAME_STATE_STOPPED;
                printf( "The game is stopped.\n" );
                return false;
            }
        
        convert_success = board_coord( g->board, formal_coord, x, y );
        
        if ( convert_success == SUCCESS ) {
            stone_placed = game_place_stone( g, *x, *y );
        } else {
            printf( "The coordinate you entered is invalid, please try again.\n" );
        }
        
    } while ( !stone_placed );
    free( formal_coord );
    
    //Switch players
        if ( g->stone == BLACK_STONE ) {
            g->stone = WHITE_STONE;
        } else {
            g->stone = BLACK_STONE;
        }
    return true;
}

void game_loop(game* g)
{
    do {
        board_print( g->board, true );
        game_update( g );
    } while ( g->state == GAME_STATE_PLAYING );
}

void game_resume(game* g)
{
    if ( g->state != GAME_STATE_STOPPED ) {
        exit( RESUME_ERR );
    } else {
        g->state = GAME_STATE_PLAYING;
        game_loop( g );
    }
}

void game_replay(game* g)
{
    game* replay = game_create( g->board->size, g->type );
    
    unsigned char moveCounter = 0;
    unsigned char g_num_moves = ( g->moves_count / sizeof( move ) );

    while ( moveCounter < g_num_moves ) {
        
        
        
        //call game_place_stone to receive message for next move if it exists
        unsigned char x = g->moves[moveCounter].x;
        unsigned char y = g->moves[moveCounter].y;
        
        game_place_stone( replay, x, y ); //Need to put games messages here
        
        //print the board unless game end conditions are met
        if ( replay->state != GAME_STATE_FORBIDDEN && replay->state != GAME_STATE_FINISHED ) {
            board_print ( replay->board, true );
        }
        
        //Switch players
        if ( replay->stone == BLACK_STONE ) {
            replay->stone = WHITE_STONE;
        } else {
            replay->stone = BLACK_STONE;
        }
        
        if ( moveCounter == g_num_moves - 1 && g->state != GAME_STATE_FORBIDDEN && g->state != GAME_STATE_FINISHED ) {
            printf( "The game is stopped.\n" );
        }
        
        //print moves so far
        print_moves( replay );
        
        //wait 1 second
        #ifndef _NOSLEEP
        sleep(1);
        #endif
        moveCounter++;
    }  //should call last move and print game end message
    
    //End on newline if the last stone was black
    if ( g->moves[g_num_moves - 1].stone == BLACK_STONE ) {
        printf( "\n" );
    }
}

void print_moves( game* replay )
{
    unsigned char num_moves = ( replay->moves_count / sizeof( move ) );
    unsigned char stone = BLACK_STONE;
    printf( "Moves:\n" );
    for ( int i = 0; i < num_moves; i++ ) {
        
        stone = replay->moves[i].stone;
        unsigned char x = replay->moves[i].x;
        unsigned char y = replay->moves[i].y;
        if ( stone == 1 ) {
            printf( "Black: " );
        } else if ( stone == 2 ) {
            printf( "  White: " );
        } else {
            exit( STONE_TYPE_ERR );
        }
        
        char* formal_coord = ( char* )malloc( 4 * sizeof( char ) );
        formal_coord[4] = '\0';
        
        board_formal_coord( replay->board, x, y, formal_coord );
        printf( "%3s", formal_coord );
        free( formal_coord );
        if ( stone == WHITE_STONE ) {
            printf( "\n" );
        }
    }
    
}
        
bool game_place_stone(game* g, unsigned char x, unsigned char y)
{
    //Check if the intersection is already occupied
    unsigned char current_occupant = board_get( g->board, x, y);
    if ( current_occupant != 0 ) {
        printf( "There is already a stone at the coordinate you entered, please try again.\n" );
        return false;
    }
    //Move will be logged, even if invalid
    //place stone in grid
    board_set( g->board, x, y, g->stone );
    //Save game
    save_move( g, x, y );
    
    unsigned char open_four_count = 0;
    unsigned char max_line = find_max_line( g, x, y, &open_four_count );
    
        //Check for a winner; If freestyle: 5 in a row of one color
        //If renju: Black wins if an exact five is created
        //          White wins if a five or overline is created, with no restrictions.
            //CHECKING THE POSSIBLE WIN SCENARIO
            if ( g->type == GAME_RENJU && max_line == FIVE_IN_A_ROW ) {
                //Enforce rules for black
                if ( g->stone == BLACK_STONE ) {
                    //Fours are allowed but no overline
                    if ( max_line > FIVE_IN_A_ROW ) {
                        g->state = GAME_STATE_FORBIDDEN;
                    } else {
                        g->state = GAME_STATE_FINISHED;
                    }
                } else { //Enforce rules for white, no restrictions
                    g->state = GAME_STATE_FINISHED;
                }
            } else if ( g->type == GAME_FREESTYLE && max_line >= FIVE_IN_A_ROW ) { //WIN SCENARIO
                g->state = GAME_STATE_FINISHED;
            } else if ( g->type == GAME_RENJU ) { //CHECKING THE NON-WIN Scenario
                //If the game is renju, check if the move creates one overline, or 2 open fours with no overline
                //If forbidden move is made, change the game state 
                if ( open_four_count > MAX_OPEN_FOURS || max_line >= FIVE_IN_A_ROW ) {
                    g->state = GAME_STATE_FORBIDDEN;
                }
            }     
                
    
        //Check if the move to make is forbidden
        //Check for a full board, i.e. a draw
        bool draw = board_is_full( g->board );
        
    //Prompt player if game state has changed
    if ( g->state == GAME_STATE_FORBIDDEN ) {
        if ( g->stone == WHITE_STONE ) {
            board_print( g->board, true );
            printf( "Game concluded, white made a forbidden move, black won.\n" );
            g->winner = BLACK_STONE;
        } else {
            board_print( g->board, true );
            printf( "Game concluded, black made a forbidden move, white won.\n" );
            g->winner = WHITE_STONE;
        }
    } else if ( g->state == GAME_STATE_FINISHED && draw ) {
        board_print( g->board, true );
        printf( "Game concluded, the board is full, draw.\n" );
    } else if ( g->state == GAME_STATE_FINISHED) {
        if ( g->stone == WHITE_STONE ) { //White wins
            board_print( g->board, true );
            printf( "Game concluded, white won.\n" );
            g->winner = WHITE_STONE;
        } else { //Black wins
            board_print( g->board, true );
            printf( "Game concluded, black won.\n" );
            g->winner = BLACK_STONE;
        }
    }
    return true;
}

static unsigned char find_max_line( const game* g, const unsigned char x, const unsigned char y, unsigned char* open_fours ) 
{
    //Create two pinters for either side of the current stone
    unsigned char neg = 0;
    unsigned char pos = 0;
    bool open_neg = false;
    bool open_pos = false;
    unsigned char current_line = 0;
    unsigned char max_line = 0;
    
    //Check both sides of the current location in North-South Directions
    open_pos = count_north( g, x, y, &pos );
    open_neg = count_south( g, x, y, &neg );
    
    //Add returned values from both sides and compare to max
    current_line = pos + neg + 1;
    if ( current_line >= max_line ) {
        max_line = current_line;
    }
    //Check for an open four and increment counter if so
    if ( current_line == FOUR_IN_A_ROW && open_neg && open_pos ) {
        (*open_fours)++;
    }
        
    
    //Check both sides of the current location in East-West Directions
    //Reset neg and pos and bools
    neg = 0;
    pos = 0;
    open_neg = false;
    open_pos = false;
    
    open_pos = count_east( g, x, y, &pos );
    open_neg = count_west( g, x, y, &neg );
    
    //Add returned values from both sides and compare to max
    current_line = pos + neg + 1;
    if ( current_line >= max_line ) {
        max_line = current_line;
    }
    //Check for an open four and increment counter if so
    if ( current_line == FOUR_IN_A_ROW && open_neg && open_pos ) {
        (*open_fours)++;
    }
    //Check both sides of the current location in NE-SW directions
    neg = 0;
    pos = 0;
    open_neg = false;
    open_pos = false;
    
    open_pos = count_northwest( g, x, y, &pos );
    open_neg = count_southeast( g, x, y, &neg );
    
    //Add returned values from both sides and compare to max
    current_line = pos + neg + 1;
    if ( current_line >= max_line ) {
        max_line = current_line;
    }
    //Check for an open four and increment counter if so
    if ( current_line == FOUR_IN_A_ROW && open_neg && open_pos ) {
        (*open_fours)++;
    }
    
    //Check both sides of the current location in NW-SE directions
    neg = 0;
    pos = 0;
    open_neg = false;
    open_pos = false;
    
    open_pos = count_northeast( g, x, y, &pos );
    open_neg = count_southwest( g, x, y, &neg );
    
    //Add returned values from both sides and compare to max
    current_line = pos + neg + 1;
    if ( current_line >= max_line ) {
        max_line = current_line;
    }
    //Check for an open four and increment counter if so
    if ( current_line == FOUR_IN_A_ROW && open_neg && open_pos ) {
        (*open_fours)++;
    }
    
    return max_line;
}

static bool count_north( const game* g, unsigned char x, unsigned char y, unsigned char* pos )
{
    bool open_ended = false;
    //Determine if traversing north violates boundaries
    bool on_board = in_bounds(g, x, y + 1 );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone above it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x, y + 1 );
    
    while ( stone_match ) {
        (*pos)++;
        y++;
        on_board = in_bounds(g, x, y + 1 );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x, y + 1 );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x, y + 1 ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_south( const game* g, unsigned char x, unsigned char y, unsigned char* neg )
{
    bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x, y - 1 );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x, y - 1 );
    
    while ( stone_match ) {
        (*neg)++;
        y--;
        on_board = in_bounds(g, x, y - 1 );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x, y - 1 );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x, y - 1 ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_west( const game* g, unsigned char x, unsigned char y, unsigned char* neg )
{
    
    bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x - 1, y );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x - 1, y );
    
    while ( stone_match ) {
        (*neg)++;
        x--;
        on_board = in_bounds(g, x - 1, y );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x - 1, y );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x - 1, y ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_east( const game* g, unsigned char x, unsigned char y, unsigned char* pos )
{
    
    bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x + 1, y );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x + 1, y );
    
    while ( stone_match ) {
        (*pos)++;
        x++;
        on_board = in_bounds(g, x + 1, y );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x + 1, y );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x + 1, y ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_northeast( const game* g, unsigned char x, unsigned char y, unsigned char* pos )
{
    bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x + 1, y + 1 );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x + 1, y + 1 );
    
    while ( stone_match ) {
        (*pos)++;
        x++;
        y++;
        on_board = in_bounds(g, x + 1, y + 1 );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x + 1, y + 1 );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x + 1, y + 1 ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_southwest( const game* g, unsigned char x, unsigned char y, unsigned char* neg )
{
   bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x - 1, y - 1 );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x - 1, y - 1 );
    
    while ( stone_match ) {
        (*neg)++;
        x--;
        y--;
        on_board = in_bounds(g, x - 1, y - 1 );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x - 1, y - 1 );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x - 1, y - 1 ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_northwest( const game* g, unsigned char x, unsigned char y, unsigned char* pos )
{
   bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x - 1, y + 1 );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x - 1, y + 1 );
    
    while ( stone_match ) {
        (*pos)++;
        x--;
        y++;
        on_board = in_bounds(g, x - 1, y + 1 );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x - 1, y + 1 );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x - 1, y + 1 ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool count_southeast( const game* g, unsigned char x, unsigned char y, unsigned char* neg )
{
    bool open_ended = false;
    //Determine if traversing violates boundaries
    bool on_board = in_bounds(g, x + 1, y - 1 );
    if ( !on_board ) {
        return open_ended;
    }
    //While the current stone matches the stone next to it, increment counter and increment coordinates
    bool stone_match = g->stone == board_get( g->board, x + 1, y - 1 );
    
    while ( stone_match ) {
        (*neg)++;
        x++;
        y--;
        on_board = in_bounds(g, x + 1, y - 1 );
        if ( on_board ) {
            stone_match = g->stone == board_get( g->board, x + 1, y - 1 );
        } else {
            break;
        }
    }
    //Check if there's an empty intersection at the end
    if ( on_board ) {
        open_ended = board_get( g->board, x + 1, y - 1 ) == EMPTY_INTERSECTION;
    }
    return open_ended;
}

static bool in_bounds( const game* g, unsigned char x, unsigned char y ) 
{
    if ( x < 0 || x >= g->board->size || y < 0 || y >= g->board->size ) {
        return false;
    } else {
        return true;
    }
}

bool save_move( game* g, const unsigned char x, const unsigned char y) 
{
    //Double the moves list memory if at capacity
    unsigned char num_moves = ( g->moves_count / sizeof( move ) );
    
    if ( num_moves == g->moves_capacity ) {
        g->moves_capacity = g->moves_capacity * 2;
        g->moves = ( move * )realloc( g->moves, g->moves_capacity * sizeof( move ) );
    }
    
    move mv = { x, y, g->stone };
    g->moves[num_moves] = mv;
    g->moves_count += sizeof( move );
    return true;
}
