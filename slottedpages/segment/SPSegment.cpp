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

bool SPSegment::update(TID tid, Record &record) {
    // find corresponding page via lookup table
    auto it = slottedPageMap.find(tid.pageID);

    // return if no page was found
    if(it == slottedPageMap.end()) return false;

    SlottedPage* sp = it->second;

    if(!sp->isValid(tid.slotID)) return false;

    // check if the length of data is different
    uint16_t prevLenBytes = sp->getLenBytes(tid.slotID);

    //      - equal: just overwrite
    //      - record lesser than before: update length, too and fragmentation
    //      - record length higher -> check if there is enough space on this page -> insert indirection(maybe via insert method?) or rearrange data


    return true;
}

Record &SPSegment::lookup(TID tid) {
    // find corresponding page via lookup table
    auto it = slottedPageMap.find(tid.pageID);

    // return if no page was found
    if(it == slottedPageMap.end()) exit(-1); //TODO

    SlottedPage* sp = it->second;

    if(!sp->isValid(tid.slotID)) exit(-1); //TODO


    if(sp->isTID(tid.slotID)){
        TID indirectionTID = sp->getIndirection(tid.slotID);
        return lookup(indirectionTID);
    }
    else {
        return sp->getRecordFromSlotID(tid.slotID);
    }

}
