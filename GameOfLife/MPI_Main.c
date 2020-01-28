#include "Game_Of_Life.h"
#include "IO.h"
#include "MPI_Communication.h"

int main(int argc, char *argv[]){

    //All process variables
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 0, .w_size = 0};
    int process_rank, system_size;
    int row_for_process = 0;

    //Process == 0 variables
    int *currentGen; //2d array mapped to 1d
    int *nextGen; //2d array mapped to 1d
    
    //Process != 0 variables
    int *rowsBuf; //3d array mapped to 1d
    int *nextGenRow; //1d
    int nRowsOfProcess; //nRows the current process will have
    
    //process != 0 START HERE
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &system_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    debug_print("Process rank:%d\n", process_rank);
    
    if(!process_rank){
        //readGen0 and alloc currentGen
        readGen("../IO/MPI/gen0.dat", &currentGen, &info);
        if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'g'){
            info.n_gen = atoi(argv[2]); //DON'T CHANGE info AFTER THIS
        }
        //alloc next gen array
        allocIntegerArray(&nextGen, info.w_size*info.h_size);
        if(system_size > info.h_size){
            fprintf(stderr, "Try again with n_cores <= number of rows of file\n");
            debug_print("System size:%d info.h_size:%d\n", system_size, info.h_size);
            MPI_Finalize();
            return 1;
        }
        //Sync info with other processes
        infoPropagation(process_rank, &info);
        //Send nRowsOfProcess and nRowsOfProcess gen0 rows to each process
        gen0send(system_size, &currentGen, info);
        //Test check to sync after gen0 send
        debug_print("Process %d Reached MPI_Barrier 2\n", process_rank);
        MPI_Barrier(MPI_COMM_WORLD); //2
        //Calculate all the generations 
        for(int gen_iterator = 1; gen_iterator <= info.n_gen; gen_iterator++){
            debug_print("Process rank: %d | Gen iteration %d\n", process_rank, gen_iterator);
            //calculate process 0 rows
            for(int row_for_process = 0;  ROW < info.h_size; row_for_process++){
                //Send to other processes the current row
                sendRows(process_rank, system_size, row_for_process, &currentGen[getCurrentGenPosition(ROW, 0)], info);
                //NOTE: process 0 already got all the rows of the previous gen, no need to receive anything!
                //Calculate new generation
                // NOTE: if ROW == 0, the address 0 does not correspond to a valid address of the arrays but it should not access it inside calculateNextGenRow()
                calculateNextGenRow(&currentGen[mapRow(ROW-1)], &nextGen[mapRow(ROW)] , info, ROW);
                debug_print("Process rank: %d | Calculated generation\n", process_rank);
            }
            //Swap currentGen with newGen
            debug_print("Process rank: %d | CurrentGen address: %p\n", process_rank, currentGen);
            debug_print("Process rank: %d | NextGen address: %p\n", process_rank, nextGen);
            int *aux = currentGen;
            currentGen = nextGen;
            nextGen = aux;
            debug_print("Process rank: %d | CurrentGen address: %p\n", process_rank, currentGen);
            debug_print("Process rank: %d | NextGen address: %p\n", process_rank, nextGen);
            //Sync all processes
            debug_print("Process %d Reached MPI_Barrier 3\n", process_rank);
            MPI_Barrier(MPI_COMM_WORLD); //3
            debug_print("Process %d Passed MPI_Barrier 3\n", process_rank);
            //Get every row of next gen and join into the array
            for(int i = 0; i < info.h_size; i++){
                if(i%system_size != 0){
                    MPI_Recv(
                        &currentGen[mapRow(i)],
                        info.w_size,
                        MPI_INT,
                        MPI_ANY_SOURCE,
                        i,
                        MPI_COMM_WORLD,
                        MPI_STATUS_IGNORE
                    );
                }
            }
            //write to the file the currentGen
            debug_print("Process %d | Received every row of gen %d | Writing to file...\n", process_rank, gen_iterator);
            writeGen("../IO/MPI/gen", currentGen, info, gen_iterator);
            debug_print("Process %d | Wrote gen %d to file\n", process_rank, gen_iterator);
        }
        debug_print("Process rank: %d | NextGen address: %p\n", process_rank, nextGen);
        free(nextGen);
        debug_print("Process rank: %d | CurrentGen address: %p\n", process_rank, currentGen);
        free(currentGen);
    }
    else{
        //Sync info
        infoPropagation(process_rank, &info);
        //Receive nRowsOfProcess and gen0 rows
        gen0recv(process_rank, system_size, &rowsBuf, &nRowsOfProcess, info);
        //Test check to sync after gen0 receive
        debug_print("Process %d Reached MPI_Barrier 2\n", process_rank);
        MPI_Barrier(MPI_COMM_WORLD); //2
        //Alloc nextGenRow
        allocIntegerArray(&nextGenRow, info.w_size);
        //Calculate and send every gen
        MPI_Request aboveRequest;
        MPI_Request belowRequest;
        MPI_Request req;
        for(int gen_iterator = 1; gen_iterator <= info.n_gen; gen_iterator++){
            debug_print("Process rank: %d | Gen iteration %d\n", process_rank, gen_iterator);
            //For every row that received initially
            for(row_for_process = 0;  row_for_process < nRowsOfProcess; row_for_process++){
                //debug_print("Process Rank: %d | Sent\n", process_rank);
                //Send row to other processes
                sendRows(
                    process_rank, 
                    system_size, 
                    row_for_process, 
                    &(rowsBuf[getRowsBufPosition(row_for_process, 1, 0)]), 
                    info
                );
                //Receive rows from other processes
                receiveRows(
                    process_rank, 
                    system_size, 
                    row_for_process, 
                    &(rowsBuf[getRowsBufPosition(row_for_process, 0, 0)]),
                    &aboveRequest,
                    &(rowsBuf[getRowsBufPosition(row_for_process, 2, 0)]), 
                    &belowRequest,
                    info
                );
                //Wait for receiveing the desired rows
                MPI_Wait(&aboveRequest, MPI_STATUS_IGNORE);
                MPI_Wait(&belowRequest, MPI_STATUS_IGNORE);
                //Calculate new generation
                calculateNextGenRow(&rowsBuf[mapRowForProcess(row_for_process)], nextGenRow, info, ROW);
                debug_print("Process Rank: %d | Calculated next gen\n", process_rank);
                //Send to process 0 the next gen row
                MPI_Isend(
                    nextGenRow,
                    info.w_size,
                    MPI_INT,
                    0,
                    ROW,
                    MPI_COMM_WORLD,
                    &req
                );
                MPI_Request_free(&req);
                //Copy nextGenRow to rowsBuf
                memcpy(&rowsBuf[getRowsBufPosition(row_for_process, 1, 0)], nextGenRow, sizeof(int)*info.w_size);
            }
            //Sync all processes
            debug_print("Process %d Reached MPI_Barrier 3\n", process_rank);
            MPI_Barrier(MPI_COMM_WORLD); //3
        }
        free(nextGenRow);
        free(rowsBuf);
    }
    //To make sure all buffers were freed before ending the program
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}