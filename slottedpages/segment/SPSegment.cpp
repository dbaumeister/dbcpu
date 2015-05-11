//
// Created by dbaumeister on 05.05.15.
//

#include "SPSegment.h"

TID SPSegment::insert(Record &record) {
    // 1. find page with enough space
    // (either slotpointer - datapointer has enough space or we have enough fragmented space -> defragment)
    // 2. if not found -> create a new page (with BufferManager.fixPage)
    // 3. Create TID with slotID & pageID
    //      and create corresponding slot
    // 4. insert record data of (record.len) to free data space
    // 5. update data pointer
    // 6. update slot (with pointer to data)
    // 7. append slot
    // 8. return TID
    return TID();
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
