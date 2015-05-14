//
// Created by dbaumeister on 12.05.15.
//

#include <iostream>
#include "SlottedPage.h"


uint16_t SlottedPage::insertNewSlot(char const* dataptr, uint16_t lenInBytes){
    Slot newSlot;

    header.dataStart -= lenInBytes;
    memcpy( &data[header.dataStart], dataptr, lenInBytes);

    newSlot.offset = header.dataStart;
    newSlot.length = lenInBytes;

    slots[header.slotCount] = newSlot;
    setControlbitsToDefault(header.slotCount);
    return header.slotCount++; //first return, then increase
}

/*
 * Only call on a regular slot (no indirection, not removed!)
 */
bool SlottedPage::tryUpdate(uint16_t slotID, char const *dataptr, uint16_t lenInBytes) {
    Slot* s = &slots[slotID];
    //1. (length is the same) -> insert at the same position
    //& 2. (length is smaller than before) -> insert at the same position  -> update length & fragmented
    if(s->length >= lenInBytes){
        memcpy(&data[s->offset], dataptr, lenInBytes);
        header.fragmentedSpace += s->length - lenInBytes;
        s->length = lenInBytes;
        return true;
    }
    else if( s->length < lenInBytes){
        //3. (length is greater than before && space on page) -> insert anywhere on page -> update length, offset, fragmented
        if(getFreeSpaceInBytes() >= lenInBytes){
            header.fragmentedSpace += s->length;

            header.dataStart -= lenInBytes;
            memcpy( &data[header.dataStart], dataptr, lenInBytes);

            s->offset = header.dataStart;
            s->length = lenInBytes;
            setControlbitsToDefault(slotID);

            return true;
        }
        //4. (length is greater than before && space after defrag) -> defrag and insert anywhere on page -> update length & offset
        else if(getFreeSpaceInBytesAfterDefrag() + s->length >= lenInBytes){
            setControlbitsToRemoved(slotID);
            defrag();

            header.dataStart -= lenInBytes;
            memcpy( &data[header.dataStart], dataptr, lenInBytes);

            s->offset = header.dataStart;
            s->length = lenInBytes;

            setControlbitsToDefault(slotID);
            return true;
        }
    }
    return false;
}

bool SlottedPage::tryUpdateSlotWithIndirection(uint16_t slotID, char const *dataptr, uint16_t lenInBytes) {
    Slot* s = &slots[slotID];
    if(getFreeSpaceInBytes() >= lenInBytes){
        header.dataStart -= lenInBytes;
        memcpy( &data[header.dataStart], dataptr, lenInBytes);

        s->offset = header.dataStart;
        s->length = lenInBytes;

        setControlbitsToDefault(slotID);
        //here we have completely removed the TID, which was previously stored in this slot
        return true;
    }
    //4. (length is greater than before && space after defrag) -> defrag and insert anywhere on page -> update length & offset
    else if(getFreeSpaceInBytesAfterDefrag() >= lenInBytes){
        defrag();

        header.dataStart -= lenInBytes;
        memcpy( &data[header.dataStart], dataptr, lenInBytes);

        s->offset = header.dataStart;
        s->length = lenInBytes;

        setControlbitsToDefault(slotID);
        //here we have completely removed the TID, which was previously stored in this slot
        return true;
    }
    return false;
}


bool SlottedPage::tryUpdateRemovedSlot(uint16_t slotID, char const *dataptr, uint16_t lenInBytes) {
    bool success = tryUpdateSlotWithIndirection(slotID, dataptr, lenInBytes);
    if(success) header.numUnusedSlots--;
    return success;
}

void SlottedPage::insertIndirection(uint16_t slotID, TID indirection) {
    memcpy(&slots[slotID], &indirection, sizeof(Slot));
}

void SlottedPage::remove(uint16_t slotID) {
    if(!isRemoved(slotID)){
        if(!isIndirection(slotID)) {
            header.fragmentedSpace += slots[slotID].length;
        }
        setControlbitsToRemoved(slotID);
        header.numUnusedSlots++;
    }
}


/*
 * Only call on non indirection and not removed
 */
Record &SlottedPage::getRecordFromSlotID(uint16_t slotID){
    Slot s = slots[slotID];
    return *new Record(s.length, &data[s.offset]);
}


TID SlottedPage::getIndirection(uint16_t slotID) {
    TID tid;
    memcpy(&tid, &slots[slotID], sizeof(TID));
    return tid;
}

uint16_t SlottedPage::getFreeSpaceInBytes(){
    return header.dataStart - header.slotCount * (uint16_t) sizeof(Slot);
}

uint16_t SlottedPage::getFreeSpaceInBytesAfterDefrag(){
    return getFreeSpaceInBytes() + header.fragmentedSpace;
}

uint16_t SlottedPage::getNumUnusedSlots() {
    return header.numUnusedSlots;
}

/*
 * You can check if you got a valid slot in return via isValid(slotID)
 */
uint16_t SlottedPage::getFirstUnusedSlot() {
    uint16_t i = (uint16_t)0;
    for(; i < header.slotCount; ++i){
        if(isRemoved(i)){
            return i;
        }
    }
    return i;
}

bool SlottedPage::hasEnoughSpace(uint16_t lenInBytes){
    return getFreeSpaceInBytes() >= lenInBytes + sizeof(Slot);
}

bool SlottedPage::hasEnoughSpaceAfterDefrag(uint16_t lenInBytes){
    return getFreeSpaceInBytes() + header.fragmentedSpace >= lenInBytes + sizeof(Slot);
}

bool SlottedPage::isIndirection(uint16_t slotID) {
    return slots[slotID].controlBits != BM_DEFAULT && slots[slotID].controlBits != BM_REMOVED;
}

bool SlottedPage::isRemoved(uint16_t slotID) {
    return slots[slotID].controlBits == BM_REMOVED;
}

bool SlottedPage::isValid(uint16_t slotID) {
    return slotID < header.slotCount;
}

/*
 * Reorders data of not removed slots to get free space
 * Returns free space in bytes after defragmentation
 */
uint16_t SlottedPage::defrag(){
    SlottedPage spWorkingCopy;
    spWorkingCopy.header.slotCount = 0;
    spWorkingCopy.header.dataStart = PAGESIZE - sizeof(SlottedPage::SPHeader);
    spWorkingCopy.header.fragmentedSpace = 0;
    spWorkingCopy.header.numUnusedSlots = 0;

    for(uint16_t i = 0; i < header.slotCount; ++i){
        if(!isRemoved(i) && !isIndirection(i)){
            Slot* s = &slots[i];
            uint16_t wsID = spWorkingCopy.insertNewSlot(&data[s->offset], s->length);
            s->offset = spWorkingCopy.slots[wsID].offset;
        }
    }
    //data is now sorted at the back -> update our SlottedPage
    header.dataStart = spWorkingCopy.header.dataStart;
    memcpy(&data[header.dataStart], &spWorkingCopy.data[spWorkingCopy.header.dataStart], PAGESIZE - sizeof(SPHeader) - spWorkingCopy.header.dataStart);
    header.fragmentedSpace = 0;
    return getFreeSpaceInBytes();
}

void SlottedPage::setControlbitsToRemoved(uint16_t slotID) {
    slots[slotID].controlBits = BM_REMOVED;
}

void SlottedPage::setControlbitsToDefault(uint16_t slotID) {
    slots[slotID].controlBits = BM_DEFAULT;
}

