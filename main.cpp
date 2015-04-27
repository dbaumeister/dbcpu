#include <stdio.h>
#include <stdint.h>

#include "buffer/BufferFrame.h"
#include "buffer/BufferManager.h"

void printArguments(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
    BufferManager bufferManager(3);
    BufferFrame bufferFrame1 = bufferManager.fixPage(1, false);
    BufferFrame bufferFrame2 = bufferManager.fixPage(2, false);
    BufferFrame bufferFrame3 = bufferManager.fixPage(3, false);
    bufferManager.unfixPage(bufferFrame1, true);
    BufferFrame bufferFrame4 = bufferManager.fixPage(4, false);
    BufferFrame bufferFrame5 = bufferManager.fixPage(5, false);

    return 0;
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}