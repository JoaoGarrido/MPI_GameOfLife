#include "ConwayGOL.h"

void allocIntegerArray(int **intArray, long n_elements){
    *intArray = malloc(sizeof(int) * n_elements);
    if(intArray == NULL){
        printf("Failed to allocate nextGen array. Exiting...\n");
        exit(0);
    }
}

int infoPropagation(int process_rank, ConwayGameOfLifeInfo *info){
    //Receive info from process 0
    MPI_Bcast( //info.n_gen
        &(*info).n_gen,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    MPI_Bcast( //info.h_size
        &(*info).h_size,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    MPI_Bcast( //info.w_size
        &(*info).w_size,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );
    //Make sure the variable were received: Test without it later
    debug_print("Process %d Reached MPI_Barrier 1\n", process_rank);
    MPI_Barrier(MPI_COMM_WORLD); //1 
    debug_print("info.w_size: %d | info.h_size: %d | n_gen: %d\n", (*info).w_size, (*info).h_size, (*info).n_gen);
    //Check if the Bcast variables were received correctly
    if(!(*info).w_size || !(*info).h_size || !(*info).n_gen){
        fprintf(stderr, "MPI_Bcast couldn't reach the other processes\n");
        MPI_Finalize();
        return 1;
    }
    return 0;
}

void gen0recv(int process_rank, int system_size, int **rowsBuf, int *nRowsOfProcess, ConwayGameOfLifeInfo info){
    int row_for_process = 0;
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
    debug_print("Process_rank: %d | nRowsOfProcess: %d\n", process_rank, *nRowsOfProcess);
    //Alloc rowsBuf
    allocIntegerArray(rowsBuf, (*nRowsOfProcess)*3*info.w_size);
    //Receive every gen0 rows that will process
    for(row_for_process = 0; row_for_process < *nRowsOfProcess; row_for_process++){
        MPI_Recv(
            &(*rowsBuf)[getRowsBufPosition(row_for_process, 1, 0)],
            info.w_size,
            MPI_INT,
            0, //Initial rows from process 0
            ROW,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
        debug_print("Process rank: %d | Row %d received\n", process_rank, ROW);
    }
}

void gen0send(int system_size, int **buf, ConwayGameOfLifeInfo info){
    int row_for_process = 0;
    int nRowsPerProcess = 0;
    debug_print("Process rank: %d | Entered gen0send\n", 0);
    for(int processes_iterator = 1; processes_iterator < system_size; processes_iterator++){
        //debug_print("Process 0 sending data to process %d\n", processes_iterator);
        nRowsPerProcess = info.h_size/system_size + (processes_iterator < ( info.h_size%system_size ) );
        //Send nRows per process
        MPI_Ssend(
            &nRowsPerProcess, 
            1,
            MPI_INT,
            processes_iterator,
            GEN_0_N_ROWS_TAG, //SPECIAL TAG
            MPI_COMM_WORLD
        );
        debug_print("Process rank: 0 | nRowsOfProcess %d sent to process: %d\n", row_for_process, processes_iterator);
        // Scatter is an option too but needs to pre-process the array 
        // so the rows each process will take are sequential
        for(row_for_process = 0; GEN_0_ROW < info.h_size; row_for_process++){
            //debug_print("Process 0 sending row %d to process %d\n", GEN_0_ROW, processes_iterator);
            MPI_Ssend(
                &(*buf)[getCurrentGenPosition(GEN_0_ROW, 0)],
                info.w_size,
                MPI_INT,
                processes_iterator,
                GEN_0_ROW, //SPECIAL TAG
                MPI_COMM_WORLD
            );
            debug_print("Row %d sent to process %d\n", GEN_0_ROW, processes_iterator);
        }
    }
}

void receiveRows(int process_rank, int system_size, int row_for_process, int* aboveRowBuf, int* belowRowBuf, ConwayGameOfLifeInfo info){
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
            info.w_size,
            MPI_INT,
            process_recv[0], 
            getRow(process_recv[0], row_for_process),
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
    //DOWN
    if(ROW >= (info.h_size - 1) ){ //If not the last row, receive from row below
        MPI_Recv(
            belowRowBuf,
            info.w_size,
            MPI_INT,
            process_recv[1], 
            getRow(process_recv[1], row_for_process),
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
    }
}

void sendRows(int process_rank, int system_size, int row_for_process, int* currentRow, ConwayGameOfLifeInfo info){
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
        MPI_Ssend(
            currentRow,
            info.w_size,
            MPI_INT,
            process_send[0], 
            ROW, 
            MPI_COMM_WORLD
        );
    }
    //BELOW
    if(ROW >= (info.h_size - 1) ){ //If not the last row, sends to next process
        MPI_Ssend(
            currentRow,
            info.w_size,
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
    if(f == NULL){
        printf("Couldn't open the file.\n");
        return -1;
    }
    //Read array size info and store it
    fgets(readBuf, bufLenght, f);
    sscanf(readBuf, "%d %d", &(*info).h_size, &(*info).w_size);
    if(!(*info).h_size || !(*info).w_size) return -1;
    //Allocate array
    allocIntegerArray(array, (*info).w_size*(*info).h_size);
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

int writeGen(char *filename, int *array, ConwayGameOfLifeInfo info, int genIterator){
    char writeBuf[1024], tokenBuf[10];
    int bufLenght = sizeof(writeBuf);
    //open file
    strcpy(writeBuf, filename);
    sprintf(tokenBuf, "%d.dat", genIterator);
    strcat(writeBuf, tokenBuf);
    FILE *f = fopen(writeBuf, "w");
    memset(writeBuf, 0, bufLenght);
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

int checkLimit(int value, int min, int max){
    if(value < min) return 1;
    else if(value > max) return 1;
    return 0;
}

int calculateNumberOfNeighbours(const int *MooreNeighbourhoodArray, const int row, const int xPosition, const ConwayGameOfLifeInfo info){
    int n = 0;
    for (int i=-1; i < 2; i++){
        for(int j=-1; j < 2; j++){
            if(!checkLimit(row+i, 0, info.h_size-1) && !checkLimit(xPosition+j, 0, info.w_size-1) ){
                /*
                printf("i:%d j:%d mapped:%d value:%d\n", 
                    i, j,
                    getCurrentGenPosition(1+i, xPosition+j),
                    MooreNeighbourhoodArray[getCurrentGenPosition(1+i, xPosition+j)]
                );
                */
                n += MooreNeighbourhoodArray[getCurrentGenPosition(1+i, xPosition+j)];
                
            }
        }
    }
    //printf("%d", MooreNeighbourhoodArray[getCurrentGenPosition(1, xPosition)]);
    n -= MooreNeighbourhoodArray[getCurrentGenPosition(1, xPosition)];
    return n;
}

void calculateNextGenRow(int *rowsBuf, int* nextGenRow, ConwayGameOfLifeInfo info, int currentGenRowNumber){
    int n = 0;
    for(int j=0; j<info.w_size; j++){
        n = calculateNumberOfNeighbours(rowsBuf, currentGenRowNumber, j, info);
        //printf("row:%d column:%d n: %d\n", currentGenRowNumber, j, n);
        if( ( rowsBuf[getCurrentGenPosition(1, j)] && (n > 1) && (n < 4) ) || (!rowsBuf[getCurrentGenPosition(1, j)] && (n == 3) ) ){
            nextGenRow[j] = 1;
        }
        else{
            nextGenRow[j] = 0;
        }
    }
}