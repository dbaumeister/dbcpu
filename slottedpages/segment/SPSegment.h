//
// Created by dbaumeister on 05.05.15.
//

#ifndef PROJECT_SPSEGMENT_H
#define PROJECT_SPSEGMENT_H

#include <stdint.h>
#include "../../buffer/BufferManager.h"
#include "../../buffer/BufferFrame.h"
#include "../Record.h"

const uint16_t REDIRECT = 1;


struct TID {
    uint16_t slotID;
    uint64_t pageID : 48;
};

struct Slot {
    uint16_t length;
    uint16_t offset : 15;
    uint16_t isRedirect : 1;
};

struct SlottedPage {
    struct SPHeader {
        uint16_t slotCount = 0;
        char* firstFreeSlot = (char*) 0; //TODO verify
        char* dataStart = (char*) PAGESIZE - sizeof(SPHeader) - 1; //TODO verify
        uint16_t fragmentedSpace = 0;//TODO optimize Header Size
    } header;
    union {
        Slot slots[(PAGESIZE - sizeof(SPHeader)) / sizeof(Slot)];
        char data[PAGESIZE - sizeof(SPHeader)];
    };

};

class SPSegment {
public:

    TID insert(Record& record);
    bool remove(TID tid);
    bool update(TID tid, Record& record);
    Record& lookup(TID tid);

private:
    std::unordered_map<uint64_t, SlottedPage&> slottedPages; //uint64_t => pageID
};

#endif //PROJECT_SPSEGMENT_H
