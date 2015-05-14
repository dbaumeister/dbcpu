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
#include "../../buffer/BufferManager.h"
#include "../../buffer/BufferFrame.h"
#include "../Record.h"


class SPSegment {
public:
    SPSegment(BufferManager& bm, uint16_t segmentID) : bufferManager(bm), slottedPageCount(0) {
        segIDShifted = ((uint64_t)segmentID) << 48;
    }

    TID insert(Record& record);
    bool remove(TID tid);
    bool update(TID tid, Record& record);
    Record& lookup(TID tid);

private:
    BufferManager& bufferManager;

    uint64_t slottedPageCount;

    uint64_t segIDShifted;

    uint64_t createID(uint64_t pageID);
    TID insertAndDoNotUseSpecificPage(Record &record, uint16_t leaveOutPage);
};

#endif //PROJECT_SPSEGMENT_H
