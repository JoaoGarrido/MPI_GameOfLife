#include "IO.h"

int readGen(char *filename, int **array, ConwayGameOfLifeInfo* info){
    char readBuf[1024];
    char *ptr_token;
    int bufLenght = sizeof(readBuf);
    int offset = 0;
    (*info).h_size = 0;
    (*info).w_size = 0;
    //Open file
    FILE* f = fopen(filename, "r");
    if(f == NULL){
        printf("Couldn't open the file.\n");
        return -1;
    }
    //Read array size info and store it
    fgets(readBuf, bufLenght, f);
    sscanf(readBuf, "%d %d", &(*info).h_size, &(*info).w_size);
    if(!(*info).h_size || !(*info).w_size) return -1;
    //Allocate array
    allocIntegerArray(array, (*info).w_size*(*info).h_size);
    //Read file to array
    for(int i = 0; i < (*info).h_size; i++){
        fgets(readBuf, bufLenght, f);
        ptr_token = readBuf;
        for(int j = 0; j < (*info).w_size; j++){
            sscanf(ptr_token, "%d%n", &((*array)[i*(*info).w_size + j]) , &offset);
            ptr_token += offset;
        }
    }
    //Close file
    fclose(f);
    return 0;
}

int writeGen(char *filename, int *array, ConwayGameOfLifeInfo info, int genIterator){
    char writeBuf[1024], tokenBuf[10];
    int bufLenght = sizeof(writeBuf);
    //open file
    strcpy(writeBuf, filename);
    sprintf(tokenBuf, "%d.dat", genIterator);
    strcat(writeBuf, tokenBuf);
    FILE *f = fopen(writeBuf, "w");
    memset(writeBuf, 0, bufLenght);
    //write file
    for(int i = 0; i < info.h_size; i++){
        memset(writeBuf, 0, bufLenght);
        for(int j = 0; j < (info.w_size-1); j++){
            sprintf(tokenBuf, "%d   ", array[getCurrentGenPosition(i, j)]);
            strcat(writeBuf, tokenBuf);
        }
        sprintf(tokenBuf, "%d\n", array[getCurrentGenPosition(i, info.w_size-1)]);
        strcat(writeBuf, tokenBuf);
        fputs(writeBuf, f);
        //debug_print("Line %d: %s", i, writeBuf);
    }
    //close file
    fclose(f);
    return 0;
}

void debug_print_array(int *array, const ConwayGameOfLifeInfo info){
    for(int i = 0; i < info.h_size; i++){
        debug_print("Line %d: ", i);
        for(int j = 0; j < info.w_size-1; j++){
            debug_print("%d ", array[getCurrentGenPosition(i, j)]);
        }
        debug_print("%d\n", array[getCurrentGenPosition(i, info.w_size-1)] );
    }
}