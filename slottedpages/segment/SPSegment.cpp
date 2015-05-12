//
// Created by dbaumeister on 05.05.15.
//

#include "SPSegment.h"

TID SPSegment::insert(Record &record) {
    // find page with enough space
    for(auto it : slottedPageMap){
        SlottedPage* sp = it.second;
        if(sp->hasEnoughSpace(record.getLen())){
            TID tid;
            tid.slotID = sp->insertData(record.getData(), record.getLen());
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
            tid.slotID = sp->insertData(record.getData(), record.getLen());
            tid.pageID = it.first;
            return tid;
        }
    }


    // 2. if not found -> create a new page (with BufferManager.fixPage)
    SlottedPage* sp = new SlottedPage();
    slottedPageMap.insert(std::pair<uint64_t, SlottedPage*>(slottedPageCount, sp));

    TID tid;
    tid.slotID = sp->insertData(record.getData(), record.getLen());
    tid.pageID = slottedPageCount;

    ++slottedPageCount;
    return tid;
}

bool SPSegment::remove(TID tid) {
    // 1. find corresponding page via lookup table
    //      return if no page was found
    // 2. set corresponding slot to invalid
    // 3. update parameter of fragmentation
    // 4. append page to fragmentedPages list, if a certain threshold is reached
    // 5. return true
    return false;
}

bool SPSegment::update(TID tid, Record &record) {
    // 1. find page via lookup
    //      return if no page was found
    // 2. check if the lenght of data varies
    //      - equal: just overwrite
    //      - record lesser than before: update length, too and fragmentation
    //      - record length higher -> check if there is enough space on this page -> insert indirection(maybe via insert method?) or rearrange data
    // 3. return true
    return false;
}

Record &SPSegment::lookup(TID tid) {
    // 1. find page
    // 2. create record (consider the slot might contain an indirection, direct data or pointer to data)

}
