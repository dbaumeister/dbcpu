#include <stdio.h>
#include <iostream>
#include <stdint.h>

#include "buffer/BufferManager.h"
#include "slottedpages/segment/SPSegment.h"

void printArguments(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
    std::cout << sizeof(Slot) << std::endl;
    return 0;
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}