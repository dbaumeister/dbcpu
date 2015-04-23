#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include "legacy/externalsort.h"
#include "collection/LockedCollection.h"
#include "buffer/BufferFrame.h"
#include "buffer/BufferFrameWrapper.h"
#include "buffer/BufferManager.h"

void printArguments(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
    BufferManager bufferManager(3);
    bufferManager.fixPage(1, false);
    return 0;
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}