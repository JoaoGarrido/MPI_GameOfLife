
#define INCORRECT_OUTPUT 1

#define CORRECT_OUTPUT 0

int test_calculateMooreNeighbourdhood(){
    return CORRECT_OUTPUT;
}

int test_readGen(){
    return CORRECT_OUTPUT;
}

int test_writeGen(){
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
    int returnTester = test_calculateMooreNeighbourdhood();
    if(returnTester) printf("Error code %d in test_calculateMooreNeighbourdhood()", returnTester);
    int returnTester = test_readGen();
    if(returnTester) printf("Error code %d in test_readGen()", returnTester);
    int returnTester = test_writeGen();
    if(returnTester) printf("Error code %d in test_writeGen()", returnTester);
    //MPI tests
    int returnTester = test_gen0_propagation();
    if(returnTester) printf("Error code %d in test_gen0_propagation()", returnTester);  
    int returnTester = test_send_receives();
    if(returnTester) printf("Error code %d in test_send_receives()", returnTester);
}