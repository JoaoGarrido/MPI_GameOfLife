#include <stdio.h>
#include <stdlib.h>

void setPointer(int *n){
    *n = 10; 
    printf("In setPointer() value %d at address %d(local: %d)\n", *n, n, &n);
}

void setArray(int **a){
    *a = malloc(sizeof(int) * 10);
    for(int i = 0; i < 10; i++)    
        (*a)[i] = i; //*(*a+i) = i;
}

void printArrayAddress(int *a){
    printf("Function:\na -> %d\n&a -> %d\n*a -> %d\n**a -> %d\n", a, &a, *a, *a);
}

int main(){
    int n = 0;
    printf("Pre setPointer(): In main() value %d at address %d\n", n, &n);
    setPointer(&n);
    printf("In main() value %d at address %d\n", n, &n);

    int *a;
    setArray(&a);
    for(int i = 0; i < 10; i++){
        printf("a[%d] = %d at address %d(or %d)\n", i, a[i], &(a[i]), a+i);
    }

    int *b;
    b = malloc(sizeof(int)*10);
    b[0] = 20;
    printArrayAddress(&b);
    printf("MAIN:\na -> %d\n&a -> %d\n*a -> %d\n", b, &b, *b);
}