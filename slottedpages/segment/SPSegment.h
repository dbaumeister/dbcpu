//
// Created by dbaumeister on 05.05.15.
//

#ifndef PROJECT_SPSEGMENT_H
#define PROJECT_SPSEGMENT_H

#include <stdint.h>
#include <iostream>

#include "SlottedPage.h"
#include "TID.h"

#include "../../buffer/BufferManager.h"
#include "../../buffer/BufferFrame.h"
#include "../Record.h"


class SPSegment {
public:
    SPSegment(BufferManager& bm) : bufferManager(bm), slottedPageCount(0) {}

    TID insert(Record& record);
    bool remove(TID tid);
    bool update(TID tid, Record& record);
    Record& lookup(TID tid);

    //~SPSegment() {
        //TODO unfix all pages
    //}

private:
    BufferManager& bufferManager;
    uint64_t slottedPageCount;

    std::unordered_map<uint64_t, SlottedPage*> slottedPageMap; //stores all Slotted Pages for fast lookups (uint64_t => pageID)

};

#endif //PROJECT_SPSEGMENT_H
