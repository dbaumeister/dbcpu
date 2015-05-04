#include <stdio.h>
#include <stdint.h>

#include "buffer/BufferFrame.h"
#include "buffer/BufferManager.h"

void printArguments(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
    return 0;
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}