//
// Created by dbaumeister on 05.05.15.
//

#include "SPSegment.h"


TID SPSegment::insert(Record &record, bool isTID) {
    // find page with enough space
    for(auto it : slottedPageMap){
        SlottedPage* sp = it.second;
        if(sp->hasEnoughSpace(record.getLen())){
            TID tid;
            tid.slotID = sp->insert(record.getData(), record.getLen(), isTID);
            tid.pageID = it.first;
            return tid;
        }
    }

    //no free space found ->search again for fragmented
    for(auto it : slottedPageMap){
        SlottedPage* sp = it.second;
        if(sp->hasEnoughSpaceAfterDefrag(record.getLen())){
            sp->defrag();
            TID tid;
            tid.slotID = sp->insert(record.getData(), record.getLen(), isTID);
            tid.pageID = it.first;
            return tid;
        }
    }


    // 2. if not found -> create a new page (with BufferManager.fixPage)
    SlottedPage* sp = new SlottedPage();//TODO: change to bufferManager.fixPage
    slottedPageMap.insert(std::pair<uint64_t, SlottedPage*>(slottedPageCount, sp));

    TID tid;
    tid.slotID = sp->insert(record.getData(), record.getLen(), isTID);
    tid.pageID = slottedPageCount;

    ++slottedPageCount;
    return tid;
}

bool SPSegment::remove(TID tid) {
    // 1. find corresponding page via lookup table
    auto it = slottedPageMap.find(tid.pageID);

    //return if no page was found
    if(it == slottedPageMap.end()) return false;

    SlottedPage* sp = it->second;

    //check if the slot is valid - only continue if it is
    if(! sp->isValid(tid.slotID)) return false;

    //check for indirection
    if(sp->isTID(tid.slotID)){
        //delete indirection
        TID indirectionTID = sp->getIndirection(tid.slotID);
        remove(indirectionTID);
    }

    //remove whatever now lies in this slot
    sp->remove(tid.slotID);
    return true;
}

// updates only non removed tids
bool SPSegment::update(TID tid, Record &record) {
    // find corresponding page via lookup table
    auto it = slottedPageMap.find(tid.pageID);

    // return if no page was found
    if(it == slottedPageMap.end()) return false;

    SlottedPage* sp = it->second;

    if(!sp->isValid(tid.slotID)) return false;

    Slot* s = &sp->slots[tid.slotID];
    if(s->isRemoved){
        //reuse this slot and insert data if possible
        if(sp->getFreeSpaceInBytes() >= record.getLen()){
            sp->insertInSlot(tid.slotID, record.getData(), record.getLen(), false);

        } else if(sp->getFreeSpaceInBytesAfterDefrag() >= record.getLen()){
            sp->defrag();
            sp->insertInSlot(tid.slotID, record.getData(), record.getLen(), false);

        } else if(sp->hasEnoughSpace(sizeof(TID))){ //otherwise insert an indirection
            TID newlyInsertedTID = insert(record, false);
            sp->insertInSlot(tid.slotID, (char const*) &newlyInsertedTID, sizeof(TID), true);

        } else if(sp->hasEnoughSpaceAfterDefrag(sizeof(TID))){ //otherwise insert an indirection
            sp->defrag();
            TID newlyInsertedTID = insert(record, false);
            sp->insertInSlot(tid.slotID, (char const*) &newlyInsertedTID, sizeof(TID), true);

        } else {
            //TODO: was machen, wenn indirection nichtmehr reinpasst
            printf("Could not update TID because there is no space for an indirection.");
            return false;
        }

    }
    else if(s->isTID){

    }

    return true;
}

/*
 * Attention!
 * Returned value has to be freed!
 */
Record &SPSegment::lookup(TID tid) {
    // find corresponding page via lookup table
    auto it = slottedPageMap.find(tid.pageID);

    // return if no page was found
    if(it == slottedPageMap.end()){
        printf("TID not found. (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        exit(-1);
    }  //TODO

    SlottedPage* sp = it->second;

    if(!sp->isValid(tid.slotID)){
        printf("SlotID of TID not valid. (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        exit(-1);
    }  //TODO

    if(sp->isTID(tid.slotID)){
        TID indirectionTID = sp->getIndirection(tid.slotID);
        return lookup(indirectionTID);
    }
    else {
        return sp->getRecordFromSlotID(tid.slotID);
    }

}
