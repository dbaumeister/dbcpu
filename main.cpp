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

void printArguments(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
    //printArguments(argc, argv);
/*
    LockedCollection<int, std::string> collection;
    collection.insert(1, "a");
    collection.insert(2, "b");
    int e = 1;
    try{
        std::cout << collection.find(e) << std::endl;
        std::cout << e << " found." << std::endl;
    } catch (int exc){
        if(exc == NOT_FOUND) std::cout << e << " not found." << std::endl;
    }
    collection.remove(e);
    try{
        std::cout << collection.find(e) << std::endl;
        std::cout << e << " found." << std::endl;
    } catch (int exc){
        if(exc == NOT_FOUND) std::cout << e << " not found." << std::endl;
    }
*/
    return 0;
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}