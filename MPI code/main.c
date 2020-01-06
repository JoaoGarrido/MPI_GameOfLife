#include <mpi.h>

#define N_GEN 2

#define GEN_0 0

void writeGen(int **A, int w, int h, int gen_number){
    //open file
    //write file
    //close file
}

int main(int argc, char const *argv[]){
    int process_rank, system_size;
    
    //test array
    int p = 4;
    int A[4][4] = {
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0},
        {1, 0, 0, 1}
    };
    int genA[4][4] = {};
    int h_size = 4;
    int w_size = 4;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &system_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if(!process_rank){
        //Send gen 0 info to all processes
        for(int processes_iterator = 1; processes_iterator < system_size; processes_iterator++){
            for(int row_for_process = 0; row_for_process*p < h_size; row_for_process++){
                int tag = processes_iterator+p*row_for_process;
                MPI_Send(
                    &A[tag],
                    w_size,
                    MPI_INT,
                    processes_iterator,
                    tag,
                    MPI_COMM_WORLD);
            }
        }
        for(int gen_iterator = 1; gen_iterator < N_GEN; gen_iterator++){
            //calculate process 0 rows
            //join mini array
            writeGen(genA, w_size, h_size, gen_iterator);
        }
    }
    else{
        //calculate process > 0 rows
    }

    MPI_Finalize();
    return 0;
}