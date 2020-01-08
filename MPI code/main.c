#include <mpi.h>
#include <stdlib.h>

#define DEFAULT_N_GEN 100

#define Convert3dTo1d(local_row, row, collumm) (local_row*n_rows + row*A_info.h_size + collumm*A_info.w_size) 

#define ROW process_rank+system_size*row_for_process
#define getRow(process_rank, row_for_process) process_rank+system_size*row_for_process

#define GEN_0_ROW processes_iterator+system_size*row_for_process
#define GEN_0_TAG 0x7FFFFFFF
#define GEN_0_N_ROWS_TAG 0x7FFFFFFE

typedef struct{
    int n_gen;
    int h_size;
    int w_size;
}ConwayGameOfLifeInfo;

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
    //int **A;
    ConwayGameOfLifeInfo A_info = {DEFAULT_N_GEN};
    int row_for_process = 0;

    //test array
    int A[4][4] = {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {1, 0, 0, 1}
    };
    int genA[4][4] = {};
    
    readGen("../IO/gen0.dat", &A, &A_info);
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'g'){
        A_info.n_gen = atoi(argv[2]); //DON'T CHANGE A_info AFTER THIS
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &system_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    
    if(!process_rank){
        //Send gen 0 info to all processes
        for(int processes_iterator = 1; processes_iterator < system_size; processes_iterator++){
            // Scatter is an option too but needs to pre-process the array 
            // so the rows each process will take are sequential
            for(row_for_process = 0; GEN_0_ROW < A_info.h_size; row_for_process++){
                MPI_Send(
                    &A[GEN_0_ROW],
                    A_info.w_size,
                    MPI_INT,
                    processes_iterator,
                    GEN_0_ROW, //SPECIAL TAG
                    MPI_COMM_WORLD);
            }
            MPI_Send(
                &row_for_process,
                1,
                MPI_INT,
                processes_iterator,
                GEN_0_N_ROWS_TAG, //SPECIAL TAG
                MPI_COMM_WORLD
            );
        }
        for(int gen_iterator = 1; gen_iterator < A_info.n_gen; gen_iterator++){
            //calculate process 0 rows
            //join mini array
            writeGen(genA, A_info.w_size, A_info.h_size, gen_iterator);
        }
    }
    else{
        //calculate process > 0 rows
        int ***rowsBuf;
        int n_rows = 0;
        //Receive number of rows that will process
        MPI_Recv(
            &n_rows,
            1,
            MPI_INT,
            0, //Initial info from process 0
            GEN_0_N_ROWS_TAG, //SPECIAL TAG
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE
        );
        //Alloc array as 1D array
        rowsBuf = malloc(n_rows*3*A_info.w_size*sizeof(int));
        //Receive every gen0 rows that will process
        for(row_for_process = 0; row_for_process < n_rows; row_for_process++){
            MPI_Recv(
                &(rowsBuf[Convert3dTo1d(row_for_process,1,0)]),
                A_info.w_size,
                MPI_INT,
                0, //Initial rows from process 0
                ROW,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE
            );
        }
        //Calculate and send every gen
        for(int i = 1; i <= A_info.n_gen; i++){
            //For every row that received initially
            for(row_for_process = 0;  row_for_process < n_rows; row_for_process++){
                //Send row to other processes
                MPI_Send(
                    &(rowsBuf[Convert3dTo1d(row_for_process, 1, 0)]),
                    A_info.w_size,
                    MPI_INT,
                    process_rank-1, 
                    ROW, 
                    MPI_COMM_WORLD
                );
                MPI_Send(
                    &(rowsBuf[Convert3dTo1d(row_for_process, 1, 0)]),
                    A_info.w_size,
                    MPI_INT,
                    process_rank+1, 
                    ROW, 
                    MPI_COMM_WORLD
                );
                //Receive rows from other processes
                //UP
                MPI_Recv(
                    &(rowsBuf[Convert3dTo1d(row_for_process, 0, 0)]),
                    A_info.w_size,
                    MPI_INT,
                    process_rank-1, 
                    getRow(process_rank-1, row_for_process),
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );
                //DOWN
                MPI_Recv(
                    &(rowsBuf[Convert3dTo1d(row_for_process, 2, 0)]),
                    A_info.w_size,
                    MPI_INT,
                    process_rank+1, 
                    getRow(process_rank+1, row_for_process),
                    MPI_COMM_WORLD,
                    MPI_STATUS_IGNORE
                );
                
                //Calculate new generation

                //Send to process 0 the next gen row
                MPI_Send(
                    &(rowsBuf[Convert3dTo1d(row_for_process,1,0)]),
                    A_info.w_size,
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