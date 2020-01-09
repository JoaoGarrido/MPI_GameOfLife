#ifndef CONWAYGOL_H_
#define CONWAYGOL_H_

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_N_GEN 100

#define getFlatRow(row) row*info.w_size
#define getCurrentGenPosition(row, column) getFlatRow(row) + column
#define getRowsBufPosition(processRowNumber, row, column) processRowNumber*(*nRowsOfProcess)*getFlatRow(row) + getCurrentGenPosition(row, column)

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

int calculateNumberOfNeighbours(int **MooreNeighbourhoodArray);

#endif