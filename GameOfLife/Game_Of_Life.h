#ifndef GAMEOFLIFE_H_
#define GAMEOFLIFE_H_

#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0

#define DEFAULT_N_GEN 100

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define mapRow(row) (row)*info.w_size
#define mapRowForProcess(row_for_proc) (row_for_proc)*info.w_size*3
#define getCurrentGenPosition(row, column) mapRow(row) + (column)
#define getRowsBufPosition(row_for_proc, row, column) mapRowForProcess(row_for_proc) + getCurrentGenPosition(row, column)

#define ROW process_rank+system_size*row_for_process
#define getRow(proc_rank, row_for_proc) (proc_rank)+system_size*(row_for_proc)

#define GEN_0_ROW processes_iterator+system_size*row_for_process
#define GEN_0_TAG 9998
#define GEN_0_N_ROWS_TAG 9999

typedef struct{
    int n_gen;
    int h_size;
    int w_size;
}ConwayGameOfLifeInfo;

void allocIntegerArray(int **intArray, long n_elements);

int checkLimit(int value, int min, int max);

int calculateNumberOfNeighbours(const int *MooreNeighbourhoodArray, const int row, const int xPosition, const ConwayGameOfLifeInfo info);

void calculateNextGenRow(int *rowsBuf, int* nextGenRow, const ConwayGameOfLifeInfo info, int currentGenRowNumber);

#endif