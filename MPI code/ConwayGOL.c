#include "ConwayGOL.h"

void gen0Propagation(int process_rank, int system_size, int **buf, int *nRowsOfProcess, ConwayGameOfLifeInfo info){
    int row_for_process = 0;
    if(!process_rank){
        for(int processes_iterator = 1; processes_iterator < system_size; processes_iterator++){
            // Scatter is an option too but needs to pre-process the array 
            // so the rows each process will take are sequential
            for(row_for_process = 0; GEN_0_ROW < info.h_size; row_for_process++){
                MPI_Send(
                    (*buf + GEN_0_ROW),
                    info.w_size,
                    MPI_INT,
                    processes_iterator,
                    GEN_0_ROW, //SPECIAL TAG
                    MPI_COMM_WORLD);
            }
            //NOTE: MAYBE row_for_process--;
            MPI_Send(
                &row_for_process, 
                1,
                MPI_INT,
                processes_iterator,
                GEN_0_N_ROWS_TAG, //SPECIAL TAG
                MPI_COMM_WORLD
            );
        }
    }
    else{
        //Receive number of rows that will process
        MPI_Recv(
            nRowsOfProcess,
            1,
            MPI_INT,
            0, //Initial info from process 0
            GEN_0_N_ROWS_TAG, //SPECIAL TAG
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
        //Alloc array as 1D array
        *buf = malloc((*nRowsOfProcess)*3*info.w_size*sizeof(int));
        //Receive every gen0 rows that will process
        for(row_for_process = 0; row_for_process < *nRowsOfProcess; row_for_process++){
            MPI_Recv(
                (*buf + getRowsBufPosition(row_for_process, 1, 0)),
                info.w_size,
                MPI_INT,
                0, //Initial rows from process 0
                ROW,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
        }
    }
}

void receiveRows(int process_rank, int system_size, int row_for_process, int* aboveRowBuf, int* belowRowBuf, ConwayGameOfLifeInfo currentGenInfo){
    int process_recv[2];
    /*
        [0] -> receive from process responsible from row above
        [1] -> receive from process responsible from row below
    */
    if(!process_rank){
        process_recv[0] = system_size-1;
        process_recv[1] = process_rank+1;
    }
    else{
        process_recv[0] = process_rank-1;
        process_recv[1] = (process_rank+1)%system_size;
    }
    //UP
    if(ROW){ //If not row 0, receive from row above
        MPI_Recv(
            aboveRowBuf,
            currentGenInfo.w_size,
            MPI_INT,
            process_recv[0], 
            getRow(process_recv[0], row_for_process),
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    //DOWN
    if(ROW >= (currentGenInfo.h_size - 1) ){ //If not the last row, receive from row below
        MPI_Recv(
            belowRowBuf,
            currentGenInfo.w_size,
            MPI_INT,
            process_recv[1], 
            getRow(process_recv[1], row_for_process),
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
}

void sendRows(int process_rank, int system_size, int row_for_process, int* currentRow, ConwayGameOfLifeInfo currentGenInfo){
    int process_send[2];
    /*
        [0] -> send to process responsible for row above
        [1] -> send to process responsible for row below
    */
    if(!process_rank){
        process_send[0] = system_size-1;
        process_send[1] = process_rank+1;
    }
    else{
        process_send[0] = process_rank-1;
        process_send[1] = (process_rank+1)%system_size;
    }
    //ABOVE
    if(ROW){
        MPI_Send(
            currentRow,
            currentGenInfo.w_size,
            MPI_INT,
            process_send[0], 
            ROW, 
            MPI_COMM_WORLD
        );
    }
    //BELOW
    if(ROW >= (currentGenInfo.h_size - 1) ){ //If not the last row, sends to next process
        MPI_Send(
            currentRow,
            currentGenInfo.w_size,
            MPI_INT,
            process_send[1],
            ROW,
            MPI_COMM_WORLD
        );
    }
}

int readGen(char *filename, int **array, ConwayGameOfLifeInfo* info){
    char readBuf[1024];
    char *ptr_token;
    int bufLenght = sizeof(readBuf);
    int offset = 0;
    (*info).h_size = 0;
    (*info).w_size = 0;
    //Open file
    FILE* f = fopen(filename, "r");
    if(f == NULL) return -1;
    //Read array size info and store it
    fgets(readBuf, bufLenght, f);
    sscanf(readBuf, "%d %d", &(*info).h_size, &(*info).w_size);
    if(!(*info).h_size || !(*info).w_size) return -1;
    //Allocate array
    *array = malloc(sizeof(int) * ((*info).h_size * (*info).w_size) );
    if(*array == NULL) return -1;
    //Read file to array
    for(int i = 0; i < (*info).h_size; i++){
        fgets(readBuf, bufLenght, f);
        ptr_token = readBuf;
        for(int j = 0; j < (*info).w_size; j++){
            sscanf(ptr_token, "%d%n", &((*array)[i*(*info).w_size + j]) , &offset);
            ptr_token += offset;
        }
    }
    //Close file
    fclose(f);
    return 0;
}

int writeGen(char *filename, int *array, ConwayGameOfLifeInfo info){
    char writeBuf[1024], tokenBuf[10];
    int bufLenght = sizeof(writeBuf);
    //open file
    FILE *f = fopen(filename, "w");
    //write file
     for(int i = 0; i < info.h_size; i++){
        memset(writeBuf, 0, bufLenght);
        for(int j = 0; j < (info.w_size-1); j++){
            sprintf(tokenBuf, "%d   ", array[getCurrentGenPosition(i, j)]);
            strcat(writeBuf, tokenBuf);
        }
        sprintf(tokenBuf, "%d\n", array[getCurrentGenPosition(i, info.w_size-1)]);
        strcat(writeBuf, tokenBuf);
        fputs(writeBuf, f);
    }
    //close file
    fclose(f);
    return 0;
}

int calculateNumberOfNeighbours(int **MooreNeighbourhoodArray){
    int n = 0;
    for (int i = 0; i < 3; i++){
        for(int j; j < 3; j++){
            // NOTE: Search for the most efficient way to cycle because of cache 
            n += MooreNeighbourhoodArray[j][i];
        }
    }
    n -= MooreNeighbourhoodArray[1][1];
    return n;
}