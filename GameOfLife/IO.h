#ifndef IO_H_
#define IO_H_

#include "Game_Of_Life.h"
#include <string.h>

int readGen(char *filename, int **array, ConwayGameOfLifeInfo* info);

int writeGen(char *filename, int *array, ConwayGameOfLifeInfo info, int genIterator);

void debug_print_array(int *array, const ConwayGameOfLifeInfo info);

#endif