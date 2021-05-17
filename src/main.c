#include "errno.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

typedef unsigned char byte;
typedef short int16;
typedef int int32;

typedef struct
{
    byte width, height, colorCount;
    int16 bitsPerPixel;
} ICONDIRENTRY;

#define RGB     2
#define ALPHA   4

#define CHANNELS(x) (1 + (x&RGB) + ((x&ALPHA)>>2))

#define LITTLE_ENDIAN16(x) (x)&0xff, (x>>8)&0xff 
#define LITTLE_ENDIAN32(x) (x)&0xff, (x>>8)&0xff, (x>>16)&0xff, (x>>24)&0xff

int main(int argc, char const *argv[])
{
    FILE* inFile = fopen(argv[1], "rb");
    if(!inFile)
    {
        printf("Could not open %s: %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }
    FILE* outFile = fopen(argv[2], "wb");
    byte fileHeader[] = {
        0, 0, 1, 0, 1, 0
    };
    fseek(inFile, 0, SEEK_END); size_t dataSize = ftell(inFile);
    fwrite(fileHeader, 1, 6, outFile);

    ICONDIRENTRY dirHeader;
    fseek(inFile, 0x13, SEEK_SET);
    fread(&(dirHeader.width), 1, 1, inFile);
    fseek(inFile, 0x17, SEEK_SET);
    fread(&(dirHeader.height), 1, 1, inFile);
    byte bitDepth;
    fread(&bitDepth, 1, 1, inFile);
    byte colorType; fread(&colorType, 1, 1, inFile);
    dirHeader.bitsPerPixel = CHANNELS(colorType) * bitDepth;
    byte buffer[16] = {
        dirHeader.width, dirHeader.height, 0, 0,
        0, 0, LITTLE_ENDIAN16(dirHeader.bitsPerPixel), 
        LITTLE_ENDIAN32(dataSize), 0x16, 0, 0, 0
    };
    fwrite(buffer, 1, 16, outFile);

    fseek(inFile, 0, 0);
    int n;
    while((n = fread(buffer, 1, 16, inFile)))
        fwrite(buffer, 1, n, outFile);

    fclose(inFile);
    fclose(outFile);
    return 0;
}