from sys import argv
from os import path

def littleEndian(x: int, size=2):
    result = []
    for _ in range(size):
        result.append(x&0xff)
        x>>=8
    return bytes(result)

class ICOHandler:
    def __init__(self, path: str, bitmapData: bytes):
        self.fileHeader = bytes([ 0, 0, 1, 0,  1, 0 ])
        self.bitmapSize = len(bitmapData)
        self.width = bitmapData[0x13]
        self.data = bitmapData
        self.height = bitmapData[0x17]
        self.bitDepth = bitmapData[0x18]
        self.filePath = path
        self.file = open(self.filePath, "wb")
    def write(self):
        self.file.write(self.fileHeader)
        colorCount = 1<<self.bitDepth
        if colorCount>=256:
            colorCount = 0
        dirHeader = bytes([self.width, self.height, colorCount, 0, 0,0]) +\
            littleEndian(self.bitDepth<<2) + littleEndian(self.bitmapSize, 4)
        offset = len(self.fileHeader) + len(dirHeader) + 4
        self.file.write(dirHeader)
        self.file.write(littleEndian(offset, 4))
        self.file.write(self.data)

    def __del__(self):
        self.file.close()


f = open(argv[1], "rb")
outPath = path.splitext(argv[1])[0] + ".ico"
bitmapData = f.read()

handler = ICOHandler(outPath, bitmapData)
handler.write()

f.close()