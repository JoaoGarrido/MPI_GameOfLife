#include "ConwayGOL.h"

#define DEBUG 1

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
        //Send info to other processes
        MPI_Bcast( //info.n_gen
            &info.n_gen,
            1,
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
        MPI_Bcast( //info.h_size
            &info.h_size,
            1,
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
        MPI_Bcast( //info.w_size
            &info.w_size,
            1,
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
        //Sync info: Test without it later
        MPI_Barrier(MPI_COMM_WORLD); //1
        debug_print("Process %d Reached MPI_Barrier 1\n", process_rank);
        //Send nRowsOfProcess and nRowsOfProcess gen0 rows to each process
        gen0send(system_size, &currentGen, info);
        //Test check to sync after gen0 send
        debug_print("Process %d Reached MPI_Barrier 2\n", process_rank);
        MPI_Barrier(MPI_COMM_WORLD); //2
        //Calculate all the generations 
        for(int gen_iterator = 1; gen_iterator <= info.n_gen; gen_iterator++){
            //calculate process 0 rows
            for(int row_for_process = 0;  ROW < info.h_size; row_for_process++){
                //Send to other processes the current row
                sendRows(process_rank, system_size, row_for_process, &currentGen[getCurrentGenPosition(ROW, 0)], info);
                //NOTE: process 0 already got all the rows of the previous gen, no need to receive anything!
                //Calculate new generation
                // NOTE: if ROW == 0, the address 0 does not correspond to a valid address of the arrays but it should not acess it inside calculateNextGenRow()
                calculateNextGenRow(&currentGen[mapRow(ROW-1)], &nextGen[mapRow(ROW-1)] , info, ROW);
            }
            //Swap currentGen with newGen
            int *aux = currentGen;
            currentGen = nextGen;
            nextGen = aux;
            //Sync all processes
            MPI_Barrier(MPI_COMM_WORLD);
            //Get every row of next gen and join into the array
            for(int i = 0; i < info.h_size; i++){
                if(i%system_size == 0){
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
            writeGen("../IO/MPI/gen", currentGen, info, gen_iterator);
        }
        free(currentGen);
        free(nextGen);
    }
    else{
        //Sync info
        debug_print("Process %d Reached MPI_Barrier 1\n", process_rank);
        //Receive info from process 0
        MPI_Bcast( //info.n_gen
            &info.n_gen,
            1,
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
        MPI_Bcast( //info.h_size
            &info.h_size,
            1,
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
        MPI_Bcast( //info.w_size
            &info.w_size,
            1,
            MPI_INT,
            0,
            MPI_COMM_WORLD
        );
        //
        MPI_Barrier(MPI_COMM_WORLD); //1 Make sure the variable were received: Test without it later
        debug_print("info.w_size: %d | info.h_size: %d | n_gen: %d\n", info.w_size, info.h_size, info.n_gen);
        //Check if the Bcast variables were received correctly
        if(!info.w_size || !info.h_size || !info.n_gen){
            fprintf(stderr, "MPI_Bcast couldn't reach the other processes\n");
            debug_print("System size:%d info.h_size:%d\n", system_size, info.h_size);
            MPI_Finalize();
            return 1;
        }
        //Receive nRowsOfProcess and gen0 rows
        gen0recv(process_rank, system_size, &rowsBuf, &nRowsOfProcess, info);
        //Test check to sync after gen0 receive
        debug_print("Process %d Reached MPI_Barrier 2\n", process_rank);
        MPI_Barrier(MPI_COMM_WORLD); //2
        //Alloc nextGenRow
        allocIntegerArray(&nextGenRow, info.w_size);
        //Calculate and send every gen
        for(int gen_iterator = 1; gen_iterator <= info.n_gen; gen_iterator++){
            //For every row that received initially
            for(row_for_process = 0;  row_for_process < nRowsOfProcess; row_for_process++){
                //Send row to other processes
                sendRows(process_rank, system_size, row_for_process, &(rowsBuf[getRowsBufPosition(row_for_process, 1, 0)]), info);
                //Receive rows from other processes
                receiveRows(process_rank, system_size, row_for_process, &(rowsBuf[getRowsBufPosition(row_for_process, 0, 0)]), &(rowsBuf[getRowsBufPosition(row_for_process, 2, 0)]), info);
                //Calculate new generation
                calculateNextGenRow(&rowsBuf[mapRowForProcess(row_for_process)], nextGenRow, info, ROW);
                //Send to process 0 the next gen row
                MPI_Ssend(
                    nextGenRow,
                    info.w_size,
                    MPI_INT,
                    0,
                    ROW,
                    MPI_COMM_WORLD
                );
                //Copy nextGenRow to rowsBuf
                memcpy(&rowsBuf[getRowsBufPosition(row_for_process, 1, 0)], nextGenRow, info.w_size);
            }
            //Sync all processes
            MPI_Barrier(MPI_COMM_WORLD);
        }
        free(nextGenRow);
        free(rowsBuf);
    }
    MPI_Finalize();
    return 0;
}