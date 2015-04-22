//
// Created by dbaumeister on 18.04.15.
//

#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <sys/fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "externalsort.h"


struct Block {
    uint64_t count; //offset in the current small block, indicating the smallest not-processed value
    uint64_t numElementsInRAM; //number of elements of this block in the RAM

    uint64_t totalOffsetBytes; //offset in the whole file for this run in bytes
    uint64_t numReads; //number of times a small block has been read
    //--> We get the offset for the next read operation by: totalOffsetBytes + numReads * numElementsInRAM * sizeof(uint64_t)

    uint64_t totalBytesRun; //total number of bytes that can be processed in this run
    //--> this block cannot read  anymore data, if it exceeds totalBytesRun reads
    //--> Stop when totalBytesRun <= numReads * numElementsInRAM * sizeof(uint64_t)

    int fd; //file descriptor of the current file;

    uint64_t* valuesInRAM; //pointer to the actual data currently in the RAM
};

void sortRuns(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);
void kWayMerge(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);


/*
 * General strategy:
 * 1. Load the inputFile block by block, sort each one and write it back
 * 2. Apply k-Way-Merge
 */
void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize){
    std::cout << "SORTING: size: " << size << " bytes with memSize: " << memSize << " bytes. " << std::endl;

    int fdTmp = open("temporary_output", O_CREAT | O_RDWR, 0777);
    if(fdTmp < 0)
    {
        std::cout << "Error! Could not open tempfile. (" << strerror(errno) << ")" << std::endl;
        return;
    }

    sortRuns(fdInput, size, fdTmp, memSize);
    kWayMerge(fdTmp, size, fdOutput, memSize);

    unlink("temporary_output"); //delete the tempfile
    close(fdTmp);
}



void printBlock(Block &block){
    std::cout << "Block:\n";
    std::cout << "count: " << block.count << "\n";
    std::cout << "numElementsInRAM: " << block.numElementsInRAM << "\n";
    std::cout << "totalOffsetBytes: " << block.totalOffsetBytes << "\n";
    std::cout << "numReads: " << block.numReads << "\n";
    std::cout << "totalBytesRun: " << block.totalBytesRun << "\n";
    std::cout << "fd: " << block.fd << "\n";
    std::cout << "valuesInRAM: " << block.valuesInRAM << "\n";
}

uint64_t getSmallestElementInBlock(Block &block){
    uint64_t smallest = std::numeric_limits<uint64_t>::max(); //get biggest number by default, just in case we have processed all values of this block already
    if(block.count < block.numElementsInRAM) {
        smallest = block.valuesInRAM[block.count];
    }
    return smallest;
}

uint64_t popSmallestElementInBlock(Block &block){
    uint64_t smallest = std::numeric_limits<uint64_t>::max(); //get biggest number by default, just in case we have processed all values of this block already
    if(block.count < block.numElementsInRAM){
        //we have still data in RAM
        smallest = block.valuesInRAM[block.count];
        block.count++;

        //check if we have to read another bunch of data, if there is any left
        if(block.count == block.numElementsInRAM
           && block.totalBytesRun > block.numReads * block.numElementsInRAM * sizeof(uint64_t)){
            //we have exceeded the small block size and we can still read data from disk
            pread(block.fd, block.valuesInRAM, block.numElementsInRAM * sizeof(uint64_t), block.totalOffsetBytes + block.numReads * block.numElementsInRAM * sizeof(uint64_t));
            block.numReads++;
            block.count = 0;
        }
    }
    return smallest;
}

/*
 * General strategy:
 * - Load smallBlocks with accumulated size of memSize/2
 * - successively get the smallest number from those blocks
 * - fill the other half of memSize with those smallest numbers until it reaches the size of a smallBlock
 * - write into output once memSize/2 is full
 */
void kWayMerge(int fdInput, uint64_t size, int fdOutput, uint64_t memSize){
    uint64_t numElementsRAM = memSize / sizeof(uint64_t); //number of elements that can fit in the RAM in total
    uint64_t numMaxElementsToWriteBack = numElementsRAM / 2; //write a bunch of numbers back, when this number of elements is reached
    uint64_t numBlocks = size / (sizeof(uint64_t)  * numElementsRAM);
    uint64_t numElementsPerBlock = numMaxElementsToWriteBack / numBlocks; //the RAM can hold this number of unsorted values additionally to
    // the sorted part

    //--> Memory in Ram consists of: numElements = numElementsToWriteBack + numBlock * numElementsPerBlock
    Block blocks[numBlocks];
    for(uint64_t i = 0; i < numBlocks; i++){
        //init blocks
        Block b;
        b.numElementsInRAM = numElementsPerBlock;
        b.fd = fdInput;
        b.totalBytesRun = size / numBlocks;
        b.totalOffsetBytes = i * size / numBlocks;
        b.count = 0;
        uint64_t*  values = (uint64_t*) malloc(b.numElementsInRAM * sizeof(uint64_t));
        b.valuesInRAM = values;
        ssize_t  s = pread(b.fd, b.valuesInRAM, b.numElementsInRAM * sizeof(uint64_t), b.totalOffsetBytes);
        if(s < 0){
            std::cout << "Error! Could not read data. (" << strerror(errno) << ")" << std::endl;
            return;
        }
        b.numReads = 1;
        blocks[i] = b;
    }

    //Do stuff with blocks
    uint64_t numElementsTotal = size / sizeof(uint64_t); //total number of values to be sorted
    for(uint64_t i = 0; i < numElementsTotal; i += numMaxElementsToWriteBack){

        uint64_t numElementsToWriteBack = std::min(numMaxElementsToWriteBack, numElementsTotal - i);
        uint64_t* values = (uint64_t*) malloc(numElementsToWriteBack * sizeof(uint64_t));

        for(uint64_t j = 0; j < numElementsToWriteBack; j++){

            Block smallestBlock = blocks[0];
            uint64_t smallestValue = getSmallestElementInBlock(smallestBlock);

            for(uint64_t k = 1; k < numBlocks; k++){ //low k expected -> loop should be sufficient for now. higher performance with e.g. BinTree

                Block b = blocks[k];
                uint64_t val = getSmallestElementInBlock(b);
                if(val <= smallestValue){
                    smallestValue = val;
                    smallestBlock = b;
                }
            }
            values[j] = popSmallestElementInBlock(smallestBlock);
        }

        //write sorted values into output file
        pwrite(fdOutput, values, numElementsToWriteBack * sizeof(uint64_t), i * sizeof(uint64_t));
    }


    //free blocks values
    for(uint64_t i = 0; i < numBlocks; i++){
        free(blocks[i].valuesInRAM);
    }

}

void sortRuns(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    uint64_t numElementsPerSweep = memSize / sizeof(uint64_t); //integer flooring
    uint64_t sizePerSweep = numElementsPerSweep * sizeof(uint64_t);

    uint64_t numSweeps = size / sizePerSweep; //integer flooring

    uint64_t i = 0;
    for(; i < numSweeps; ++i){
        uint64_t* values = (uint64_t*) malloc(sizePerSweep);
        ssize_t sizeRead = pread(fdInput, values, sizePerSweep, i * sizePerSweep);

        std::sort(values, values + sizeRead / sizeof(uint64_t));

        pwrite(fdOutput, values, sizeRead, i * sizePerSweep);

        free(values);
    }
    uint64_t rest = size - numSweeps * sizePerSweep;
    if(rest > 0) { //if there are leftovers (e.g. if size is smaller than memSize)
        uint64_t* values = (uint64_t*) malloc(rest);
        ssize_t sizeRead = pread(fdInput, values, rest, i * sizePerSweep);

        std::sort(values, values + sizeRead / sizeof(uint64_t));

        pwrite(fdOutput, values, sizeRead, i * sizePerSweep);

        free(values);
    }
}

bool isFileSorted(int fd, uint64_t size, uint64_t memSize) {
    std::cout << "VERIFY SORTING: size: " << size << " bytes with memSize: " << memSize << " bytes. " << std::endl;

    uint64_t numElementsPerSweep = memSize / sizeof(uint64_t); //integer flooring
    uint64_t sizePerSweep = numElementsPerSweep * sizeof(uint64_t);

    uint64_t numSweeps = size / sizePerSweep; //integer flooring

    uint64_t lastTestedValue = 0; //0 is smallest uint

    uint64_t i = 0;
    for(; i < numSweeps; ++i){
        uint64_t* values = (uint64_t*) malloc(sizePerSweep);
        pread(fd, values, sizePerSweep, i * sizePerSweep);

        for(int j = 0; j < numElementsPerSweep; j++){
            //verify that nextValue is >= the lastTestedValue
            if(values[j] >= lastTestedValue) {
                lastTestedValue = values[j];
            } else {
                free(values);
                return false; //otherwise the numbers are not sorted -> we can abort
            }
        }
        free(values);
    }
    uint64_t rest = size - numSweeps * sizePerSweep;
    uint64_t numElementsLastSweep = rest / sizeof(uint64_t);

    if(rest > 0) { //if there are leftovers
        uint64_t* values = (uint64_t*) malloc(rest);
        pread(fd, values, rest, i * sizePerSweep);

        for(int j = 0; j < numElementsLastSweep; j++){
            //verify that nextValue is >= the lastTestedValue
            if(values[j] >= lastTestedValue) {
                lastTestedValue = values[j];
            } else {
                free(values);
                return false; //otherwise the numbers are not sorted -> we can abort
            }
        }
        free(values);
    }
    //if not aborted earlier -> everything is sorted
    return true;
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
