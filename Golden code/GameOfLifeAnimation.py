import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation


def readGen(path, genNumber):
    f = open(path + str(genNumber) + ".dat", "r")
    if genNumber == 0:
        readGen.ySize, readGen.xSize = [int(returnValues) for returnValues in f.readline().split()]
        readGen.currentGen = np.zeros((readGen.ySize, readGen.xSize), dtype = np.uint8)
    for y in range(readGen.ySize):
        line = f.readline()
        readGen.currentGen[y, :] = [np.uint8(returnValues) for returnValues in line.split()]
    f.close()
    return readGen.currentGen

def plotAnimation(path, n, savePath):
    fig, ax = plt.subplots() 
    ims = []
    for i in range(n):
        im = ax.imshow(readGen(path, i), interpolation='nearest')
        ims.append([im])
    ani = animation.ArtistAnimation(fig, ims, interval = 400, repeat_delay = 1000)
    ani.save(savePath, fps=60, writer='ffmpeg') #NOTE: Try to make it better
    plt.show()

def main():
    NUM_GEN = 100
    plotAnimation("../IO/GoldenGen/gen", NUM_GEN, "../IO/Animations/ConwayGameOfLife_Golden.mp4")
    #plotAnimation("../IO/MPI/gen", NUM_GEN, "../IO/Animations/ConwayGameOfLife_MPI.mp4")

if __name__ == "__main__":
    main()