from GameOfLife import *

def main():
    i=0
    for i in range(0, 101):
        goldenGen = readGen("../IO/GoldenGen/gen", i)
        mpiGen = readGen("../IO/MPI/gen", i)
        if(not (goldenGen == mpiGen).all()):
            print("Different values in gen ", i)
        else:
            print("Correct values in gen ", i)

if __name__ == "__main__":
    main()