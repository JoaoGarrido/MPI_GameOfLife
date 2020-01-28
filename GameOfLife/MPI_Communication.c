#include "MPI_Communication.h"

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

void receiveRows(int process_rank, int system_size, int row_for_process, int* aboveRowBuf, MPI_Request *aboveRequest, int* belowRowBuf, MPI_Request *belowRequest, ConwayGameOfLifeInfo info){
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
        MPI_Irecv(
            aboveRowBuf,
            info.w_size,
            MPI_INT,
            process_recv[0], 
            ROW-1,
            MPI_COMM_WORLD,
            aboveRequest
        );
        debug_print("Process rank: %d | Received row %d from process %d\n", process_rank, ROW-1, process_recv[0]);
    }
    //DOWN
    if((ROW) < (info.h_size - 1) ){ //If not the last row, receive from row below
        MPI_Irecv(
            belowRowBuf,
            info.w_size,
            MPI_INT,
            process_recv[1], 
            ROW+1,
            MPI_COMM_WORLD,
            belowRequest
        );
        debug_print("Process rank: %d | Received row %d from process %d\n", process_rank, ROW+1, process_recv[1]);
    }
}

void sendRows(int process_rank, int system_size, int row_for_process, int* currentRow, ConwayGameOfLifeInfo info){
    MPI_Request req;
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
    //If the process below or above is process 0, no need to send because he already has the row
    //If row 0 send only to process responsible for row below
    if((ROW) && process_send[0] != 0){
        MPI_Isend(
            currentRow,
            info.w_size,
            MPI_INT,
            process_send[0], 
            ROW, 
            MPI_COMM_WORLD,
            &req
        );
        MPI_Request_free(&req);
        debug_print("Process rank: %d | Sent below row %d to process %d\n", process_rank, ROW, process_send[0]);
    }
    //If last row send only to process responsible for row above
    if((ROW) < (info.h_size - 1) && process_send[1] != 0){ //If not the last row or the process is 0, sends to next process
        MPI_Isend(
            currentRow,
            info.w_size,
            MPI_INT,
            process_send[1],
            ROW,
            MPI_COMM_WORLD,
            &req
        );
        MPI_Request_free(&req);
        debug_print("Process rank: %d | Sent above row %d to process %d\n", process_rank, ROW, process_send[1]);
    }
}