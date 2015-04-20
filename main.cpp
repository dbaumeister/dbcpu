#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <cstdlib>

#include "externalsort.h"


void printArguments(int argc, const char* argv[]);
void applyArguments(int argc, const char* argv[]);

int main(int argc, const char* argv[])
{
    //printArguments(argc, argv);
    applyArguments(argc, argv);
    return 0;
}


void applyArguments(int argc, const char* argv[]){
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "sort") == 0 && argc > i + 3) {
            //sort <inputFile> <outputFile> <memoryBufferInMB>

            int fdInput = open(argv[i+1], O_RDONLY);
            if(fdInput < 0) {
                printf("Could not open file %s\n", argv[i+1]);
                break;//continue with next argument, which is hopefully valid
            }

            struct stat64 file_status;
            fstat64(fdInput, &file_status);

            uint64_t size = (uint64_t) file_status.st_size;

            int fdOutput = open(argv[i+2], O_CREAT | O_RDWR);
            if(fdOutput < 0) {
                if(close(fdInput) < 0) {
                    printf("Could not close file %s\n", argv[i+1]);
                }
                printf("Could not open file %s\n", argv[i+2]);
                break;
            }

            uint64_t memSize = (uint64_t) atoi(argv[i+3]);
            externalSort(fdInput, size, fdOutput, memSize);
            bool isSorted = isFileSorted(fdOutput, size, memSize);
            isSorted ? printf("SUCCESS: File %s is now sorted!", argv[i+2]) : printf("FAIL: File is not sorted.");

            if(close(fdInput) < 0) {
                printf("Could not close file %s\n", argv[i+1]);
            }
            if(close(fdOutput) < 0) {
                printf("Could not close file %s\n", argv[i+2]);
            }
            i += 3;
        }
        //check other arguments here
    }
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}