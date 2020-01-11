#include "ConwayGOL.h"

#define VISUAL_CHECK 0

#define INCORRECT_OUTPUT 1

#define CORRECT_OUTPUT 0

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

int test_calculateNewGen(){
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
        calculateNewGen(&(array[mapRow(i)]), &nextGen[mapRow(i+1)], info, i);
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
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN};
    if(readGen("../IO/MPI/gen0.dat", &currentGen, &info)) 
        return INCORRECT_OUTPUT;
    if(info.h_size != 17 || info.w_size != 38)  
        return INCORRECT_OUTPUT;
    //VisualCheck
    #if VISUAL_CHECK == 1
    for(int i = 0; i < info.h_size; i++){
        for(int j = 0; j < info.w_size; j++){
            printf("%d  ", currentGen[getCurrentGenPosition(i, j)]);
        }
        printf("\n");
    }
    #endif
    free(currentGen);
    return CORRECT_OUTPUT;
}

int test_writeGen(){
    int *currentGen;
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN};
    //Use readGen to test
    if(readGen("../IO/MPI/gen0.dat", &currentGen, &info)) 
        return INCORRECT_OUTPUT;
    if(writeGen("../IO/MPI/TEST", currentGen, info, 0))
        return INCORRECT_OUTPUT;
    return CORRECT_OUTPUT;
}

int test_gen0_propagation(){
    return CORRECT_OUTPUT;
}

int test_send_receives(){
    return CORRECT_OUTPUT;
}

int main(){
    //Regular tests
    int returnTester = test_checktLimit();
    if(returnTester) printf("Error code %d in test_checktLimit()\n", returnTester);
    //returnTester = test_calculateMooreNeighbourdhood();
    //if(returnTester) printf("Error code %d in test_calculateMooreNeighbourdhood()\n", returnTester);
    returnTester = test_calculateNewGen();
    if(returnTester) printf("Error code %d in test_calculateNewGen()\n", returnTester);
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