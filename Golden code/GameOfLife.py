import numpy as np
from GameOfLifeAnimation import *

NUM_GEN = 3

def updateGen(currentGen):
    ySize, xSize = currentGen.shape
    nextGen = np.zeros((ySize, xSize), dtype = np.uint8)
    #iterate over every cell
    #NOTE: CHECK OUTPUT
    for y in range(ySize):
        for x in range(xSize):
            number_of_neighboors_alive = 0        
            #calculate number of neighboors alive of the current cell
            for x_aux in range(x-1,x+2):
                for y_aux in range(y-1, y+2):
                    if x_aux >= 0 and y_aux >= 0:
                        if x_aux < xSize and y_aux < ySize:
                            number_of_neighboors_alive += currentGen[y_aux, x_aux]
            #create next generation 
            if (currentGen[y, x]) and (number_of_neighboors_alive > 1) and (number_of_neighboors_alive < 4) or (not currentGen[y, x]) and (number_of_neighboors_alive == 3):
                nextGen[y, x] = 1
    return nextGen[1:ySize+1, 1:xSize+1]

def writeGen(currentGen, genNumber):
    f = open("../IO/GoldenGen/gen" + str(genNumber) + ".dat", "w")
    #NOTE: SEARCH MORE EFFICIENT WAY OF DOING THIS
    f.write( np.array2string(currentGen, max_line_width = 200, separator = "   ").replace("[","").replace("]","").replace("\n ", "\n") )
    f.close()
    return

def main():
    currentGen = readGen(0)
    for i in range(1, NUM_GEN):
        currentGen = updateGen(currentGen)
        writeGen(currentGen, i)
        #updateAnimation()

if __name__ == "__main__":
    main()