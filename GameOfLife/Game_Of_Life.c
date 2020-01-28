#include "Game_Of_Life.h"

void allocIntegerArray(int **intArray, long n_elements){
    *intArray = malloc(sizeof(int) * n_elements);
    if(*intArray == NULL){
        printf("Failed to allocate nextGen array. Exiting...\n");
        exit(0);
    }
}

int checkLimit(int value, int min, int max){
    if(value < min) return 1;
    else if(value > max) return 1;
    return 0;
}

int calculateNumberOfNeighbours(const int *MooreNeighbourhoodArray, const int row, const int xPosition, const ConwayGameOfLifeInfo info){
    int n = 0;
    for (int i=-1; i < 2; i++){
        for(int j=-1; j < 2; j++){
            if(!checkLimit(row+i, 0, info.h_size-1) && !checkLimit(xPosition+j, 0, info.w_size-1) ){
                debug_print("i:%d j:%d mapped:%d value:%d\n", 
                    i, j,
                    getCurrentGenPosition(1+i, xPosition+j),
                    MooreNeighbourhoodArray[getCurrentGenPosition(1+i, xPosition+j)]
                );
                n += MooreNeighbourhoodArray[getCurrentGenPosition(1+i, xPosition+j)];
            }
        }
    }
    //printf("%d", MooreNeighbourhoodArray[getCurrentGenPosition(1, xPosition)]);
    n -= MooreNeighbourhoodArray[getCurrentGenPosition(1, xPosition)];
    return n;
}

void calculateNextGenRow(int *rowsBuf, int* nextGenRow, const ConwayGameOfLifeInfo info, int currentGenRowNumber){
    int n = 0;
    for(int j=0; j<info.w_size; j++){
        n = calculateNumberOfNeighbours(rowsBuf, currentGenRowNumber, j, info);
        debug_print("row:%d column:%d n: %d\n", currentGenRowNumber, j, n);
        if( ( rowsBuf[getCurrentGenPosition(1, j)] && (n > 1) && (n < 4) ) || (!rowsBuf[getCurrentGenPosition(1, j)] && (n == 3) ) ){
            nextGenRow[j] = 1;
        }
        else{
            nextGenRow[j] = 0;
        }
    }
}