//
// Created by dbaumeister on 05.05.15.
//

#include "SPSegment.h"


TID SPSegment::insert(Record &record) {
    // find page with enough space
    //TODO additionally check for removed slots
    for(uint16_t i = 0; i < slottedPageCount; ++i){
        BufferFrame* bufferFrame = bufferManager.fixPage(createID(i), true);
        SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

        if(sp->getNumUnusedSlots() > 0) {
            uint16_t slotID = sp->getFirstUnusedSlot();
            if(sp->isValid(slotID)){
                if(sp->tryUpdateRemovedSlot(slotID, record.getData(), record.getLen())){
                    //inserting in slot was successful
                    TID tid;
                    tid.slotID = slotID;
                    tid.pageID = i;
                    bufferManager.unfixPage(bufferFrame, true);
                    return tid;
                }
            }
        }

        if(sp->hasEnoughSpace((uint16_t) record.getLen())){
            TID tid;
            tid.slotID = sp->insertNewSlot(record.getData(), (uint16_t)record.getLen());
            tid.pageID = i;
            bufferManager.unfixPage(bufferFrame, true);
            return tid;
        }
        else if(sp->hasEnoughSpaceAfterDefrag((uint16_t)record.getLen())){ //no free space found ->search again for fragmented
            sp->defrag();
            TID tid;
            tid.slotID = sp->insertNewSlot(record.getData(), (uint16_t)record.getLen());
            tid.pageID = i;
            bufferManager.unfixPage(bufferFrame, true);
            return tid;
        }
        bufferManager.unfixPage(bufferFrame, false);
    }

    // If not found -> create a new page (with BufferManager.fixPage)
    BufferFrame* bufferFrame = bufferManager.fixPage(createID(slottedPageCount), true);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();
    TID tid;
    tid.slotID = sp->insertNewSlot(record.getData(), (uint16_t)record.getLen());
    tid.pageID = slottedPageCount;

    ++slottedPageCount;
    bufferManager.unfixPage(bufferFrame, true);
    return tid;
}

bool SPSegment::remove(TID tid) {

    BufferFrame* bufferFrame = bufferManager.fixPage(createID(tid.pageID), true);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    //check if the slot is valid - only continue if it is
    if(! sp->isValid(tid.slotID)) {
        bufferManager.unfixPage(bufferFrame, false);
        return false;
    }

    //check for indirection
    if(sp->isIndirection(tid.slotID)){
        //delete indirection
        TID indirectionTID = sp->getIndirection(tid.slotID);
        remove(indirectionTID);
    }

    //remove whatever now lies in this slot
    sp->remove(tid.slotID);
    bufferManager.unfixPage(bufferFrame, true);
    return true;
}

// updates only non removed tids
bool SPSegment::update(TID tid, Record &record) {

    BufferFrame* bufferFrame = bufferManager.fixPage(createID(tid.pageID), true);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    if(!sp->isValid(tid.slotID)) {
        bufferManager.unfixPage(bufferFrame, false);
        return false;
    }

    if(sp->isRemoved(tid.slotID)){
        bool success = sp->tryUpdateSlotWithIndirection(tid.slotID,  record.getData(), (uint16_t) record.getLen());
        if(success){
            bufferManager.unfixPage(bufferFrame, true);
            return true;
        } else {
            TID indirectionTID = insert(record);
            sp->insertIndirection(tid.slotID, indirectionTID);
            bufferManager.unfixPage(bufferFrame, true);
            return true;
        }
    }

    if(sp->isIndirection(tid.slotID)){
        TID indirectionTID = sp->getIndirection(tid.slotID);
        bool success = sp->tryUpdateSlotWithIndirection(tid.slotID, record.getData(), (uint16_t) record.getLen());
        if(success){
            remove(indirectionTID);
            bufferManager.unfixPage(bufferFrame, true);
            return true;
        } else {
            //try to fit the update into the indirection
            BufferFrame* bufferFrameIndir = bufferManager.fixPage(createID(indirectionTID.pageID), true);
            SlottedPage* spIndir = (SlottedPage*) bufferFrameIndir->getData();

            success = spIndir->tryUpdate(indirectionTID.slotID, record.getData(), (uint16_t)record.getLen());
            if(success) {
                bufferManager.unfixPage(bufferFrameIndir, true);
                bufferManager.unfixPage(bufferFrame, false);
                return true;
            }
            else {
                spIndir->remove(indirectionTID.slotID);
                bufferManager.unfixPage(bufferFrameIndir, true);
                indirectionTID = insert(record);
                sp->insertIndirection(tid.slotID, indirectionTID);
                bufferManager.unfixPage(bufferFrame, true);
                return true;
            }
        }
    }

    //no indirection and not removed -> update here
    bool success = sp->tryUpdate(tid.slotID,  record.getData(), (uint16_t) record.getLen());
    if(success){
        bufferManager.unfixPage(bufferFrame, true);
        return true;
    } else {
        TID indirectionTID = insert(record);
        sp->insertIndirection(tid.slotID, indirectionTID);
        bufferManager.unfixPage(bufferFrame, true);
        return true;
    }
}

/*
 * Attention!
 * Returned value has to be freed!
 */
Record &SPSegment::lookup(TID tid) {

    BufferFrame* bufferFrame = bufferManager.fixPage(createID(tid.pageID), false);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    if(!sp->isValid(tid.slotID)){
        printf("SlotID of TID not valid. (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        throw new std::invalid_argument("SlotID of TID not valid.");
    }

    if(sp->isIndirection(tid.slotID)){
        TID indirectionTID = sp->getIndirection(tid.slotID);
        Record* r = &lookup(indirectionTID);
        bufferManager.unfixPage(bufferFrame, false);
        return *r;
    }
    else {
        Record* r = &sp->getRecordFromSlotID(tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        return *r;
    }

}

uint64_t SPSegment::createID(uint64_t pageID) {
    return segIDShifted + pageID;
}
