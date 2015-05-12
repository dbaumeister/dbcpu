//
// Created by dbaumeister on 12.05.15.
//

#include "SlottedPage.h"


uint16_t SlottedPage::insert(char const* dataptr, uint16_t lenInBytes){
    Slot newSlot;

    header.dataStart -= lenInBytes;
    memcpy( &data[header.dataStart], dataptr, lenInBytes);

    newSlot.offset = header.dataStart;
    newSlot.length = lenInBytes;

    //TODO controlbits

    slots[header.slotCount] = newSlot;
    return header.slotCount++; //first return, then increase!!
}


bool SlottedPage::tryUpdate(uint16_t slotID, char const *dataptr, uint16_t lenInBytes) {
    //1. (length is the same) -> insert at the same position
    //2. (length is smaller than before) -> insert at the same position  -> update length & fragmented
    //3. (length is greater than before && space on page) -> insert anywhere on page -> update length, offset, fragmented
    //4. (length is greater than before && space after defrag) -> defrag and insert anywhere on page -> update length & offset
    //5. else return false
    return false;
}



void SlottedPage::insertIndirection(uint16_t slotID, TID indirection) {
    memcpy(&slots[slotID], &indirection, sizeof(TID));
    //TODO set control bits
}

void SlottedPage::remove(uint16_t slotID) {
    //TODO only add fragmentedSpace when not removed already and no indirection
    header.fragmentedSpace += slots[slotID].length;
    //TODO controlbits removed
}


/*
 * Only call on non indirection
 */
Record &SlottedPage::getRecordFromSlotID(uint16_t slotID){
    Slot s = slots[slotID];
    return *new Record(s.length, &data[s.offset]);
}


bool SlottedPage::hasEnoughSpace(uint16_t lenInBytes){
    return getFreeSpaceInBytes() >= lenInBytes + sizeof(Slot);
}

bool SlottedPage::hasEnoughSpaceAfterDefrag(uint16_t lenInBytes){
    return getFreeSpaceInBytes() + header.fragmentedSpace >= lenInBytes + sizeof(Slot);
}


uint16_t SlottedPage::getFreeSpaceInBytes(){
    return header.dataStart - header.slotCount * (uint16_t) sizeof(Slot);
}

uint16_t SlottedPage::getFreeSpaceInBytesAfterDefrag(){
    return getFreeSpaceInBytes() + header.fragmentedSpace;
}

uint16_t SlottedPage::getLenBytes(uint16_t slotID) {
    return slots[slotID].length;
}

TID SlottedPage::getIndirection(uint16_t slotID) {
    TID tid;
    memcpy(&tid, &slots[slotID], sizeof(TID));
    return tid;
}


bool SlottedPage::isIndirection(uint16_t slotID) {
    Slot s = slots[slotID];
    return false; //TODO s.isTID == TRUE && !isRemoved(slotID);
}

bool SlottedPage::isRemoved(uint16_t slotID) {
    Slot s = slots[slotID];
    //TODO controlbits removed
    return  false;
}

bool SlottedPage::isValid(uint16_t slotID) {
    Slot s = slots[slotID];
    return slotID < header.slotCount;
}

/*
 * Reorders data of not removed slots to get free space
 * Returns free space in bytes after defragmentation
 */
uint16_t SlottedPage::defrag(){
    SlottedPage spWorkingCopy;
    for(int i = 0; i < header.slotCount; ++i){
        if(!isRemoved(i)){
            Slot* s = &slots[i];
            uint16_t wsID = spWorkingCopy.insert(&data[s->offset], s->length);
            s->offset = spWorkingCopy.slots[wsID].offset;
        }
    }
    //data is now sorted at the back -> update our SlottedPage
    header.dataStart = spWorkingCopy.header.dataStart;
    memcpy(&data[header.dataStart], &spWorkingCopy.data[spWorkingCopy.header.dataStart], PAGESIZE - sizeof(SPHeader) - spWorkingCopy.header.dataStart);
    header.fragmentedSpace = 0;
    return getFreeSpaceInBytes();
}

