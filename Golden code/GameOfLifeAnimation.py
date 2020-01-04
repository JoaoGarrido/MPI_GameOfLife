import numpy as np

def readGen(genNumber):
    f = open("../IO/GoldenGen/gen" + str(genNumber) + ".dat", "r")
    
    if genNumber == 0:
        readGen.ySize, readGen.xSize = [int(returnValues) for returnValues in f.readline().split()]
        readGen.currentGen = np.zeros((readGen.ySize, readGen.xSize), dtype = np.uint8)
    for y in range(readGen.ySize):
        line = f.readline()
        readGen.currentGen[y, :] = [np.uint8(returnValues) for returnValues in line.split()]
    f.close()
    return readGen.currentGen

def updateAnimation():
    return

def main():
    for i in range(NUM_GEN):
        readGen(i)
        updateAnimation()

if __name__ == "__main__":
    main()