#ifndef MPI_COMMUNICATION_H_
#define MPI_COMMUNICATION_H_

#include "Game_Of_Life.h"
#include <mpi.h>

int infoPropagation(int process_rank, ConwayGameOfLifeInfo *info);

void gen0send(int system_size, int **buf, ConwayGameOfLifeInfo info);

void gen0recv(int process_rank, int system_size, int **rowsBuf, int *nRowsOfProcess, ConwayGameOfLifeInfo info);

void sendRows(int process_rank, int system_size, int row_for_process, int* currentRow, ConwayGameOfLifeInfo info);

void receiveRows(int process_rank, int system_size, int row_for_process, int* aboveRowBuf, MPI_Request *aboveRequest, int* belowRowBuf, MPI_Request *belowRequest, ConwayGameOfLifeInfo info);

#endif