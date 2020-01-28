from GameOfLife import *

def main():
    i=0
    for i in range(0, 5):
        goldenGen = readGen("../IO/GoldenGen/gen", i)
        mpiGen = readGen("../IO/MPI/gen", i)
        if(np.array_equal(goldenGen, mpiGen)):
            print("Correct values in gen ", i)
        else:
            print("Different values in gen ", i)

if __name__ == "__main__":
    main()