#ifndef _IO_H_
#define _IO_H_
#include "game.h"

/**
 * Imports a saved game from the designated path. Exits with error if file cannot be read.
 * @param path Path to the file to import.
 * @return A newly created game object from the designated file.
 */
game* game_import(const char* path);


void game_export(game* g, const char* path);
#endif