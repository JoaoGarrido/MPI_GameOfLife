#include "Game_Of_Life.h"
#include "IO.h"

int main(int argc, char *argv[]){
    ConwayGameOfLifeInfo info = {.n_gen = DEFAULT_N_GEN, .h_size = 0, .w_size = 0};
    int *currentGen; //2d array mapped to 1d
    int *nextGen; //2d array mapped to 1d
    readGen("../IO/GoldenGenC/gen0.dat", &currentGen, &info);
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'g'){
        info.n_gen = atoi(argv[2]); //DON'T CHANGE info AFTER THIS
    }
    debug_print("%d %d\n", info.h_size, info.w_size);
    debug_print_array(currentGen, info);
    allocIntegerArray(&nextGen, info.w_size*info.h_size);
    for(int genIterator = 1; genIterator <= info.n_gen; genIterator++){
        for(int i = 0; i < info.h_size; i++){
            calculateNextGenRow(&currentGen[mapRow(i-1)], &nextGen[mapRow(i)] , info, i);
        }
        debug_print_array(nextGen, info);
        writeGen("../IO/GoldenGenC/gen", nextGen, info, genIterator);
        int *aux = currentGen;
        currentGen = nextGen;
        nextGen = aux;
    }
    
    return 0;
}