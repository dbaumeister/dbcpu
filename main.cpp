#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <assert.h>

#include "buffer/BufferManager.h"
#include "slottedpages/segment/SPSegment.h"

void printArguments(int argc, const char* argv[]);
void testSlottedPageInsertWithRandomInserts(SlottedPage& sp);
void testSlottedPageRemove(SlottedPage& sp);

int main(int argc, const char* argv[])
{
    SlottedPage sp;

    std::cout << "Size of Header: " << sizeof(sp.header) << std::endl;
    std::cout << "Size of Slot: " << sizeof(Slot) << std::endl;
    std::cout << "Size of TID: " << sizeof(TID) << std::endl;

    testSlottedPageInsertWithRandomInserts(sp);
    testSlottedPageRemove(sp);

    std::cout << "Free space after defrag: ";
    std::cout << sp.defrag() << " bytes." << std::endl;
    return 0;
}


void testSlottedPageRemove(SlottedPage& sp){

    std::cout << "Fragmented space: " << sp.header.fragmentedSpace << " bytes." << std::endl;
    for(uint16_t i = 0; i < sp.header.slotCount; ++i){
        sp.removeData(i);
        std::cout << "Fragmented space: " << sp.header.fragmentedSpace << " bytes." << std::endl;
    }
}

void testSlottedPageInsertWithRandomInserts(SlottedPage& sp){
    void* data1 = malloc(PAGESIZE);
    for(int i = 0; ;++i){

        std::cout << "Free space left: " << sp.getFreeSpaceInBytes() << " bytes." << std::endl;

        uint16_t lenInBytes = ((double)rand() / RAND_MAX) * 64;
        if(!sp.hasEnoughSpace(lenInBytes)) {
            std::cout << "Thats not enough for " << lenInBytes
                << " bytes. (Consider Slot size of " << sizeof(Slot) << " bytes.)" << std::endl;
            break;
        }

        std::cout << "Insert " << lenInBytes << " bytes." << std::endl;
        uint16_t slotID = sp.insertData(data1, lenInBytes);
        std::cout << "Got slotID: " << slotID << std::endl;
        assert(slotID == i);
    }

    free(data1);
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}