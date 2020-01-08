#include <mpi.h>
#include <stdlib.h>

#define DEFAULT_N_GEN 100

#define getCurrentGenPosition(row, column) row*currentGenInfo.h_size + column*currentGenInfo.w_size
#define getRowsBufPosition(processRowNumber, row, column) processRowNumber*(*nRowsOfProcess) + getCurrentGenPosition(row, column)

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

void gen0Propagation(int process_rank, int system_size, int **buf, int *nRowsOfProcess, ConwayGameOfLifeInfo currentGenInfo){
    int row_for_process = 0;
    if(!process_rank){
        for(int processes_iterator = 1; processes_iterator < system_size; processes_iterator++){
            // Scatter is an option too but needs to pre-process the array 
            // so the rows each process will take are sequential
            for(row_for_process = 0; GEN_0_ROW < currentGenInfo.h_size; row_for_process++){
                MPI_Send(
                    (*buf + GEN_0_ROW),
                    currentGenInfo.w_size,
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
        *buf = malloc((*nRowsOfProcess)*3*currentGenInfo.w_size*sizeof(int));
        //Receive every gen0 rows that will process
        for(row_for_process = 0; row_for_process < nRowsOfProcess; row_for_process++){
            MPI_Recv(
                (*buf + getRowsBufPosition(row_for_process, 1, 0)),
                currentGenInfo.w_size,
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

int ** readGen(char *filename, int **array, ConwayGameOfLifeInfo* info){
    //Read array size info and store int
    //Allocate array
    //Open file
    //Read file to array
    //Close file
}

void writeGen(int **A, int w, int h, int gen_number){
    //open file
    //write file
    //close file
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
    
    readGen("../IO/gen0.dat", &currentGen, &currentGenInfo);
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'g'){
        currentGenInfo.n_gen = atoi(argv[2]); //DON'T CHANGE currentGenInfo AFTER THIS
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &system_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    int *nRowsOfProcess;
    int *rowsBuf;

    //NOTE: Find a smarter way of doing this
    if(!process_rank){
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