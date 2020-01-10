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
    int array[] = {
        1, 0, 1, 1,
        0, 1, 0, 0,
        1, 1, 0, 1,
        0, 1, 1, 1
    };
    int nextGen[] = {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0 ,0 ,0
    };
    int nextGenCorrect[] = {
        0, 1, 1, 0,
        0, 0, 0, 1,
        1, 0, 0, 1,
        1, 1, 0, 1,
    };
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 4, .w_size = 4};
    int n = 0;
    //printf("%d %d\n", info.h_size, info.w_size);
    for(int i=0; i<info.h_size; i++){
        for(int j=0; j<info.w_size; j++){
            n = calculateNumberOfNeighbours(&array, j, i, info);
            //printf("%d ", n);
            if( ( array[getCurrentGenPosition(i, j)] && (n > 1) && (n < 4) ) || (!array[getCurrentGenPosition(i, j)] && (n == 3) ) )
                nextGen[getCurrentGenPosition(i, j)] = 1;
            //printf("%d ", nextGen[getCurrentGenPosition(i, j)] );
            if(nextGen[getCurrentGenPosition(i, j)] != nextGenCorrect[getCurrentGenPosition(i, j)]){
                printf("row: %d\ncolumn:%d\n", i, j);
                return INCORRECT_OUTPUT;
            }
        }
        //printf("\n");
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
    returnTester = test_calculateMooreNeighbourdhood();
    if(returnTester) printf("Error code %d in test_calculateMooreNeighbourdhood()\n", returnTester);
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