//
// Created by dbaumeister on 05.05.15.
//

#ifndef PROJECT_SPSEGMENT_H
#define PROJECT_SPSEGMENT_H

#include <iostream>
#include <stdexcept>
#include <stdint.h>

#include "SlottedPage.h"
#include "TID.h"
#include "../buffer/BufferManager.h"
#include "../buffer/BufferFrame.h"
#include "../schema/Record.h"

#define SLOTID_OUT_OF_BOUNDS_EXCEPTION 1
#define SLOTID_TO_REMOVED_SLOT_EXCEPTION 2

class SPSegment {
public:
    SPSegment(BufferManager& bm, uint16_t segmentID) : bufferManager(bm), slottedPageCount(0) {
        segIDShifted = ((uint64_t)segmentID) << 48;
    }

    TID insert(Record& record);
    bool remove(TID tid);
    bool update(TID tid, Record& record);
    Record& lookup(TID tid);

    uint64_t getPageCount() {
        return slottedPageCount;
    }

private:
    BufferManager& bufferManager;

    uint64_t slottedPageCount;

    uint64_t segIDShifted;

    uint64_t createID(uint64_t pageID);
};

#endif //PROJECT_SPSEGMENT_H
