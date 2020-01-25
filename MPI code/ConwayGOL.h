#ifndef CONWAYGOL_H_
#define CONWAYGOL_H_

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 1

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define DEFAULT_N_GEN 100

#define mapRow(row) (row)*info.w_size
#define mapRowForProcess(row_for_proc) (row_for_proc)*info.w_size*3
#define getCurrentGenPosition(row, column) mapRow(row) + (column)
#define getRowsBufPosition(row_for_proc, row, column) mapRowForProcess(row_for_proc) + getCurrentGenPosition(row, column)


#define ROW process_rank+system_size*row_for_process
#define getRow(proc_rank, row_for_proc) (proc_rank)+system_size*(row_for_proc)

#define GEN_0_ROW processes_iterator+system_size*row_for_process
#define GEN_0_TAG 9998
#define GEN_0_N_ROWS_TAG 9999

#define THIS_GEN_WORK_DONE 0x7FFFFFFD

typedef struct{
    int n_gen;
    int h_size;
    int w_size;
}ConwayGameOfLifeInfo;

void allocIntegerArray(int **intArray, long n_elements);

int infoPropagation(int process_rank, ConwayGameOfLifeInfo *info);

void gen0send(int system_size, int **buf, ConwayGameOfLifeInfo info);

void gen0recv(int process_rank, int system_size, int **rowsBuf, int *nRowsOfProcess, ConwayGameOfLifeInfo info);

void receiveRows(int process_rank, int system_size, int row_for_process, int* aboveRowBuf, MPI_Request *aboveRequest, int* belowRowBuf, MPI_Request *belowRequest, ConwayGameOfLifeInfo info);

void sendRows(int process_rank, int system_size, int row_for_process, int* currentRow, ConwayGameOfLifeInfo info);

int readGen(char *filename, int **array, ConwayGameOfLifeInfo* info);

int writeGen(char *filename, int *array, ConwayGameOfLifeInfo info, int genIterator);

int checkLimit(int value, int min, int max);

int calculateNumberOfNeighbours(const int *MooreNeighbourhoodArray, const int row, const int xPosition, const ConwayGameOfLifeInfo info);

void calculateNextGenRow(int *rowsBuf, int* nextGenRow, ConwayGameOfLifeInfo info, int currentGenRowNumber);

#endif