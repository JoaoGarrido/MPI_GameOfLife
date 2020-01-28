#include "Game_Of_Life.h"
#include "IO.h"
#include "MPI_Communication.h"


#define INCORRECT_OUTPUT 1

#define CORRECT_OUTPUT 0

int currentGen0[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int test_allocGenArray(){
    int *genArray;
    ConwayGameOfLifeInfo info = {.w_size = 100, .h_size = 100};
    allocIntegerArray(&genArray, info.w_size*info.h_size);
    free(genArray);
    return CORRECT_OUTPUT;
}

int test_checktLimit(){
    int value = 2;
    if(checkLimit(value, 0, 1) == 0) return INCORRECT_OUTPUT;
    if(checkLimit(value, 0, 3) == 1) return INCORRECT_OUTPUT;
    value = -1;
    if(checkLimit(value, 0, 2) == 0) return INCORRECT_OUTPUT;
    if(checkLimit(value, -1, 3) == 1) return INCORRECT_OUTPUT;
    return CORRECT_OUTPUT;
}

int test_calculateMooreNeighbourdhood(){
    int rowsBuf[] = {
        0, 1, 0, 0,
        1, 1, 0, 1,
        0, 1, 1, 1
    };
    int n_neighbourds_normal[] = {
        3, 4, 6, 2
    };
    int n_neighbourds_min[] = {
        2, 3, 5, 2
    };
    int n_neighbourds_max[] = {
        2, 2, 3, 0
    };    
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 4, .w_size = 4};
    int n = 0;
    //printf("%d %d\n", info.h_size, info.w_size);
    for(int j=0; j<info.w_size; j++){
        n = calculateNumberOfNeighbours(rowsBuf, 1, j, info);
        if(n != n_neighbourds_normal[j]){
            //printf("row: normal\ncolumn:%d\nn should be %d instead of %d\n", j, n_neighbourds_normal[j], n);
            return INCORRECT_OUTPUT;
        }
        n = calculateNumberOfNeighbours(rowsBuf, 0, j, info);
        if(n != n_neighbourds_min[j]){
            printf("row: min\ncolumn:%d\nn should be %d instead of %d\n", j, n_neighbourds_min[j], n);
            return INCORRECT_OUTPUT;
        }
        n = calculateNumberOfNeighbours(rowsBuf, info.h_size - 1, j, info);
        if(n != n_neighbourds_max[j]){
            printf("row: max\ncolumn:%d\nn should be %d instead of %d\n", j, n_neighbourds_max[j], n);
            return INCORRECT_OUTPUT;
        }
    }
    return CORRECT_OUTPUT;
}

int test_calculateNextGenRow(){
    int array[] = {
        1, 1, 1, 1, //simulate rowsBuf
        1, 0, 1, 1,
        0, 1, 0, 0,
        1, 1, 0, 1,
        0, 1, 1, 1,
        1, 1, 1, 1  //simulate rowsBuf
    };
    int nextGen[] = {
        1, 1, 1, 1, //simulate rowsBuf
        1, 1, 1, 1, 
        1, 1, 1, 1, 
        1, 1, 1, 1,         
        1, 1, 1, 1, 
        1, 1, 1, 1 //simulate rowsBuf
    };
    int nextGenCorrect[] = {
        1, 1, 1, 1, //simulate rowsBuf
        0, 1, 1, 0,
        0, 0, 0, 1,
        1, 0, 0, 1,
        1, 1, 0, 1,
        1, 1, 1, 1  //simulate rowsBuf
    };
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 4, .w_size = 4};    
    for(int i = 0; i<(info.w_size); i++){
        calculateNextGenRow(&(array[mapRow(i)]), &nextGen[mapRow(i+1)], info, i);
    }
    for(int i = 0; i<(info.w_size+2);i++){
        for (int j = 0; j < info.w_size; j++){
            if(nextGen[getCurrentGenPosition(i, j)] != nextGenCorrect[getCurrentGenPosition(i, j)]){
                printf("row: %d\ncolumn:%d\n", i, j);
                return INCORRECT_OUTPUT;
            }
        }
    }
  
    return CORRECT_OUTPUT;
}

int test_readGen(){
    int *currentGen;
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 0, .w_size = 0};
    if(readGen("../IO/life.dat", &currentGen, &info)) 
        return INCORRECT_OUTPUT;
    if(info.h_size != 17 || info.w_size != 38)  
        return INCORRECT_OUTPUT;
    //int flag = 0; 
    for(int i = 0; i < info.h_size; i++){
        for(int j = 0; j < info.w_size; j++){
            if(currentGen[getCurrentGenPosition(i, j)] != currentGen0[getCurrentGenPosition(i, j)]){
                printf("ERROR Row: %d | Column: %d\n", i, j);
                //flag = 1;
                return INCORRECT_OUTPUT;
            }
        }
        //if(!flag) debug_print("Row %d is correct\n", i);
    }
    free(currentGen);
    return CORRECT_OUTPUT;
}

int test_writeGen(){
    int *currentGen;
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN};
    //Use readGen to test
    if(readGen("../IO/life.dat", &currentGen, &info)) 
        return INCORRECT_OUTPUT;
    if(writeGen("../IO/TEST", currentGen, info, 0))
        return INCORRECT_OUTPUT;
    return CORRECT_OUTPUT;
}

int test_gen0_propagation(){
    int process_rank, system_size;
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 0, .w_size = 0};
    int *rowsBuf; //3d array mapped to 1d
    int nRowsOfProcess; //nRows the current process will have    
    int *currentGen; //2d array mapped to 1d
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &system_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    if(!process_rank){
        //readGen0 and alloc currentGen
        readGen("../IO/MPI/gen0.dat", &currentGen, &info);
        //alloc next gen array
        if(system_size > info.h_size){
            fprintf(stderr, "Try again with n_cores <= number of rows of file\n");
            debug_print("System size:%d info.h_size:%d\n", system_size, info.h_size);
            MPI_Finalize();
            return INCORRECT_OUTPUT;
        }
        //Sync info with other processes
        infoPropagation(process_rank, &info);
        //Send nRowsOfProcess and nRowsOfProcess gen0 rows to each process
        gen0send(system_size, &currentGen, info);
        //Test check to sync after gen0 send
        debug_print("Process %d Reached MPI_Barrier 2\n", process_rank);
        MPI_Barrier(MPI_COMM_WORLD); //2
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
        //Test values received
        int flag = 0;
        for(int row_for_process = 0; (ROW) < info.h_size; row_for_process++){
            for(int i = 0; i < info.w_size; i++){
                if(rowsBuf[getRowsBufPosition(row_for_process, 1, i)] != currentGen0[getCurrentGenPosition(ROW, i)]){
                    printf("ERROR Process rank: %d | Row: %d | Column: %d\n", process_rank, ROW, i);
                    flag = 1; 
                    return INCORRECT_OUTPUT;
                }
            }
            if(!flag) debug_print("Process rank: %d | Row %d is correct\n", process_rank, ROW);
            flag = 0;
        }
        free(rowsBuf);
    }
    MPI_Barrier(MPI_COMM_WORLD); //3
    MPI_Finalize();
    return CORRECT_OUTPUT;
}

int test_send_receives(){
    return CORRECT_OUTPUT;
}

int main(){
    //Regular tests
    int returnTester = test_checktLimit();
    if(returnTester) printf("Error code %d in test_checktLimit()\n", returnTester);
    returnTester = test_calculateMooreNeighbourdhood();
    if(returnTester) printf("Error code %d in test_calculateMooreNeighbourdhood()\n", returnTester);
    returnTester = test_calculateNextGenRow();
    if(returnTester) printf("Error code %d in test_calculateNextGenRow()\n", returnTester);
    returnTester = test_allocGenArray();
    if(returnTester) printf("Error code %d in test_allocGenArray()\n", returnTester);
    returnTester = test_readGen();
    if(returnTester) printf("Error code %d in test_readGen()\n", returnTester);
    returnTester = test_writeGen();
    if(returnTester) printf("Error code %d in test_writeGen()\n", returnTester);
    //MPI tests
    returnTester = test_gen0_propagation();
    if(returnTester) printf("Error code %d in test_gen0_propagation()\n", returnTester);  
    returnTester = test_send_receives();
    if(returnTester) printf("Error code %d in test_send_receives()\n", returnTester);
}