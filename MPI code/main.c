#include "ConwayGOL.h"

int main(int argc, char const *argv[]){
    int process_rank, system_size;
    int *currentGen; //2D
    //int *nextGen; //2D
    ConwayGameOfLifeInfo currentGenInfo = {DEFAULT_N_GEN};
    int row_for_process = 0;

    //test array
    int A[4][4] = {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {1, 0, 0, 1}
    };
    int genA[4][4] = {};
    


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &system_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    int *nRowsOfProcess;
    int *rowsBuf;

    //NOTE: Find a smarter way of doing this
    if(!process_rank){
        readGen("../IO/gen0.dat", &currentGen, &currentGenInfo);
        if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'g'){
            currentGenInfo.n_gen = atoi(argv[2]); //DON'T CHANGE currentGenInfo AFTER THIS
        }
        gen0Propagation(process_rank, system_size, &currentGen, NULL, currentGenInfo);
    }
    else{
        gen0Propagation(process_rank, system_size, &rowsBuf, &nRowsOfProcess, currentGenInfo);
    }
    
    if(!process_rank){
        //Calculate all the generations 
        for(int gen_iterator = 1; gen_iterator <= currentGenInfo.n_gen; gen_iterator++){
            //calculate process 0 rows
            for(int row_for_process = 0;  ROW < currentGenInfo.h_size; row_for_process++){
                //Send to other processes the current row
                sendRows(process_rank, system_size, row_for_process, &currentGen[getCurrentGenPosition(ROW, 0)], currentGenInfo);
                //NOTE: process 0 already got all the rows of the previous gen, no need to receive anything!
                //Receive from other processes 
                receiveRows(process_rank, system_size, row_for_process, &currentGen[getCurrentGenPosition(ROW, 0)], &currentGen[getCurrentGenPosition(ROW, 0)], currentGenInfo);
            }
            /* SYNC WORK
            MPI_Bcast(
                ,
                1,
                MPI_INT,
                0,
                MPI_COMM_WORLD

            );*/
            //get every row of next gen and join into the array
            
            //currentGen = newGen;
            //write to the file the currentGen
            writeGen(currentGen, currentGenInfo.w_size, currentGenInfo.h_size, gen_iterator);
        }
    }
    else{
        //Calculate and send every gen
        for(int i = 1; i <= currentGenInfo.n_gen; i++){
            //For every row that received initially
            for(row_for_process = 0;  row_for_process < nRowsOfProcess; row_for_process++){
                //Send row to other processes
                sendRows(process_rank, system_size, row_for_process, &(rowsBuf[getRowsBufPosition(row_for_process, 1, 0)]), currentGenInfo);
                //Receive rows from other processes
                receiveRows(process_rank, system_size, row_for_process, &(rowsBuf[getRowsBufPosition(row_for_process, 0, 0)]), &(rowsBuf[getRowsBufPosition(row_for_process, 2, 0)]), currentGenInfo);
                //Calculate new generation

                //Send to process 0 the next gen row
                MPI_Send(
                    &(rowsBuf[getRowsBufPosition(row_for_process,1,0)]),
                    currentGenInfo.w_size,
                    MPI_INT,
                    0,
                    ROW,
                    MPI_COMM_WORLD
                );
            }
        }
        free(rowsBuf);
    }

    MPI_Finalize();
    return 0;
}