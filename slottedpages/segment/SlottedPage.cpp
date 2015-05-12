//
// Created by dbaumeister on 12.05.15.
//

#include "SlottedPage.h"


uint16_t SlottedPage::insert(char const* dataptr, uint16_t lenInBytes, bool isTID){
    Slot newSlot;

    header.dataStart -= lenInBytes;
    memcpy( &data[header.dataStart], dataptr, lenInBytes);

    newSlot.offset = header.dataStart;
    newSlot.length = lenInBytes;

    if(isTID){
        newSlot.isTID = TRUE;
    } else newSlot.isTID = FALSE;

    newSlot.isRemoved = FALSE;

    slots[header.slotCount] = newSlot;
    return header.slotCount++; //first return, then increase!!
}


Record &SlottedPage::getRecordFromSlotID(uint16_t slotID){
    Slot s = slots[slotID];
    return *new Record(s.length, &data[s.offset]);


}

void SlottedPage::remove(uint16_t slotID) {
    header.fragmentedSpace += slots[slotID].length;
    slots[slotID].isRemoved = TRUE;
}


bool SlottedPage::hasEnoughSpace(uint16_t lenInBytes){
    return getFreeSpaceInBytes() >= lenInBytes + sizeof(Slot);
}

//Test if we can insert lenInBytes data
bool SlottedPage::hasEnoughSpaceAfterDefrag(uint16_t lenInBytes){
    return getFreeSpaceInBytes() + header.fragmentedSpace >= lenInBytes + sizeof(Slot);
}

uint16_t SlottedPage::getFreeSpaceInBytes(){
    return header.dataStart - header.slotCount * sizeof(Slot);
}

uint16_t SlottedPage::getLenBytes(uint16_t slotID) {
    return slots[slotID].length;
}

TID SlottedPage::getIndirection(uint16_t slotID) {
    Slot s = slots[slotID];
    TID tid;
    memcpy(&tid, &data[s.offset], sizeof(TID));
    return tid;
}

bool SlottedPage::isDataPtr(uint16_t slotID) {
    Slot s = slots[slotID];
    return s.isTID == FALSE && !isRemoved(slotID);
}

bool SlottedPage::isTID(uint16_t slotID) {
    Slot s = slots[slotID];
    return s.isTID == TRUE && !isRemoved(slotID);
}

bool SlottedPage::isRemoved(uint16_t slotID) {
    Slot s = slots[slotID];
    return  s.isRemoved == TRUE;
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
        Slot* s = &slots[i];
        if(s->isRemoved == FALSE){
            uint16_t wsID = spWorkingCopy.insert(&data[s->offset], s->length, s->isTID == TRUE);
            Slot ws = spWorkingCopy.slots[wsID];
            s->offset = ws.offset;
        }
    }
    //data is now sorted at the back -> update our SlottedPage
    header.dataStart = spWorkingCopy.header.dataStart;
    memcpy(&data[header.dataStart], &spWorkingCopy.data[spWorkingCopy.header.dataStart], PAGESIZE - sizeof(SPHeader) - spWorkingCopy.header.dataStart);
    header.fragmentedSpace = 0;
    return getFreeSpaceInBytes();
}
