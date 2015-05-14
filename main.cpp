#include <stdio.h>
#include <iostream>

#include "buffer/BufferManager.h"
#include "slottedpages/segment/SPSegment.h"

void printArguments(int argc, const char* argv[]);
void testSlottedPageInsertWithRandomInserts(SlottedPage& sp);
void testSlottedPageRemove(SlottedPage& sp);
void testSlottedPage();
void testSPSegment();

int main(int argc, const char* argv[])
{

    std::cout << "Size of SlottedPage: " << sizeof(SlottedPage) << std::endl;
    std::cout << "Size of Header: " << sizeof(SlottedPage::SPHeader) << std::endl;
    std::cout << "Size of Slot: " << sizeof(Slot) << std::endl;
    std::cout << "Size of TID: " << sizeof(TID) << std::endl;

    testSPSegment();
    return 0;
}


void testSPSegment(){
    BufferManager* bm = new BufferManager(64);
    uint16_t numSegments = 4;
    SPSegment* spSegments[numSegments];

    for(uint16_t i = 0; i <  numSegments; ++i){
        spSegments[i] = new SPSegment(*bm, i+1);
    }

    std::vector<TID> tupleIDs[numSegments];

    size_t len = 120;
    void* dataptr = malloc(1024);
    Record record(len, (const char*)dataptr);
    Record recordBig(len + 2000, (const char*)dataptr);
    Record recordSmall(len - 2, (const char*)dataptr);

    TID tid = spSegments[0]->insert(record);
    TID tid2 = spSegments[0]->insert(recordBig);
    TID tid3 = spSegments[0]->insert(recordBig);
    spSegments[0]->update(tid, recordBig);
    spSegments[0]->remove(tid);



    free(dataptr);

    for(uint16_t i = 0; i <  numSegments; ++i){
        delete(spSegments[i]);
    }
    delete(bm);
    //Randomly fill segments with more than
}

void testSlottedPage(){
    SlottedPage sp;
    testSlottedPageInsertWithRandomInserts(sp);
    testSlottedPageRemove(sp);
    testSlottedPageInsertWithRandomInserts(sp);
    testSlottedPageRemove(sp);
}

void testSlottedPageRemove(SlottedPage& sp){

    std::cout << "Fragmented space: " << sp.header.fragmentedSpace << " bytes." << std::endl;
    for(uint16_t i = 0; i < sp.header.slotCount; ++i){
        sp.remove(i);
        std::cout << "Fragmented space: " << sp.header.fragmentedSpace << " bytes." << std::endl;
    }
    std::cout << "Free space after defrag: ";
    std::cout << sp.defrag() << " bytes." << std::endl;
}

void testSlottedPageInsertWithRandomInserts(SlottedPage& sp){
    char* data1 = (char*)malloc(PAGESIZE);
    for(int i = 0; ;++i){

        std::cout << "Free space left: " << sp.getFreeSpaceInBytes() << " bytes." << std::endl;

        uint16_t lenInBytes = ((double)rand() / RAND_MAX) * 64;
        if(!sp.hasEnoughSpace(lenInBytes)) {
            std::cout << "Thats not enough for " << lenInBytes
                << " bytes. (Consider Slot size of " << sizeof(Slot) << " bytes.)" << std::endl;
            break;
        }

        std::cout << "Insert " << lenInBytes << " bytes." << std::endl;
        uint16_t slotID = sp.insertNewSlot(data1, lenInBytes);
        std::cout << "Got slotID: " << slotID << std::endl;
    }

    free(data1);
}

void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}