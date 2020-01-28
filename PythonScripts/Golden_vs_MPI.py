from GameOfLife import *

def main():
    i=0
    correct = True
    for i in range(0, 101):
        goldenGen = readGen("../IO/GoldenGenC/gen", i)
        mpiGen = readGen("../IO/MPI/gen", i)
        if(not np.array_equal(goldenGen, mpiGen)):
            correct = False
            print("Different values in gen ", i)
    if(correct):
        print("Values are correct")

if __name__ == "__main__":
    main()