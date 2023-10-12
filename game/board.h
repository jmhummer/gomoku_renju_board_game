#ifndef _BOARD_H_
#define _BOARD_H_
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define EMPTY_INTERSECTION 0
#define BLACK_STONE 1
#define WHITE_STONE 2
#define clear() printf("\033[H\033[J")

typedef struct {
    unsigned char size;
    unsigned char* grid;
} board;

/**
 * Dynamically allocates memory to creat a new board struct. Initializes size if input is valid, exiting with
 * an error if not. All intersections set to EMPTY_INTERSECTION.
 * @param size The size to make the new board (length of one side)
 * @return The newly created board struct.
 */
board* board_create(unsigned char size);


/**
 * Frees the memore of the board struct and it's grid.
 * If the given pointer is null, exits with NULL_POINTER_ERR.
 * @param b A pointer to the board object to free.
 */
void board_delete(board* b);

/**
 * Prints the board to the standard output.
 * @param b The board struct to print out.
 * @param in_place Clears the console before printing if true.
 */
void board_print(board* b, bool in_place);

/**
 * Converts X and Y coordinates to a Letter-Number combination familiar to the player. 
 * Stores the results in formal_coord.
 * @param b Reference to the current board.
 * @param x The row index of the coordinate to convert.
 * @param y The column index of the coordinate to convert.
 * @param formal_coord The reference to the location where the formal coordinates are stored.
 * @return 0 If the program executes successfully.
 */
unsigned char board_formal_coord(board* b, unsigned char x, unsigned char y, char* formal_coord);

/**
 * Converts formal coordinates to board X and Y coordinates.
 * Stores the results in the x and y pointers.
 * @param b Reference to the current board.
 * @param formal_coord Reference to the formal coordinates to be converted.
 * @param x Reference to the x coordinate result from the conversion.
 * @param y Reference to the x coordinate result from the conversion.
 * @return 0 if the program executed successfully.
 */
unsigned char board_coord(board* b, const char* formal_coord, unsigned char* x, unsigned char* y);

/**
 * Returns the status of the given intersection of a given board.
 * @param b Reference to the current board.
 * @param x The horizontal coordinate of the desired intersection.
 * @param y The vertical coordinate of the desired intersection.
 * @return The intersection occupation state at the specified coordinates.
 */
unsigned char board_get(board* b, unsigned char x, unsigned char y);

/**
 * Stores the given state to the given coordinates on the board.
 * @param b Reference to the current board.
 * @param x The horizontal coordinate of the desired intersection.
 * @param y The vertical coordinate of the desired intersection.
 * @param stone The assignment for the given intersection.
 */
void board_set(board* b, unsigned char x, unsigned char y, unsigned char stone);

/**
 * Determines if all intersections on the current board are assigned.
 * @return True if all intersections are assigned, false otherwise.
 */
bool board_is_full(board* b);
#endif
