#ifndef CONWAYGOL_H_
#define CONWAYGOL_H_

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_N_GEN 100

#define mapRow(row) (row)*info.w_size
#define mapRowForProcess(row_for_proc) (row_for_proc)*info.w_size*info.h_size
#define getCurrentGenPosition(row, column) mapRow(row) + column
#define getRowsBufPosition(row_for_proc, row, column) mapRowForProcess(row_for_proc) + getCurrentGenPosition(row, column)

#define ROW process_rank+system_size*row_for_process
#define getRow(process_rank, row_for_process) process_rank+system_size*row_for_process

#define GEN_0_ROW processes_iterator+system_size*row_for_process
#define GEN_0_TAG 0x7FFFFFFF
#define GEN_0_N_ROWS_TAG 0x7FFFFFFE

#define THIS_GEN_WORK_DONE 0x7FFFFFFD

typedef struct{
    int n_gen;
    int h_size;
    int w_size;
}ConwayGameOfLifeInfo;

void gen0Propagation(int process_rank, int system_size, int **buf, int *nRowsOfProcess, ConwayGameOfLifeInfo info);

void receiveRows(int process_rank, int system_size, int row_for_process, int* aboveRowBuf, int* belowRowBuf, ConwayGameOfLifeInfo info);

void sendRows(int process_rank, int system_size, int row_for_process, int* currentRow, ConwayGameOfLifeInfo info);

int readGen(char *filename, int **array, ConwayGameOfLifeInfo* info);

int writeGen(char *filename, int *array, ConwayGameOfLifeInfo info, int genIterator);

int checkLimit(int value, int min, int max);

int calculateNumberOfNeighbours(const int *MooreNeighbourhoodArray, const int row, const int xPosition, const ConwayGameOfLifeInfo info);

void calculateNewGen(int *rowsBuf, int* newGenRow, ConwayGameOfLifeInfo info, int currentGenRowNumber);

#endif