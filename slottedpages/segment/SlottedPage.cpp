//
// Created by dbaumeister on 12.05.15.
//

#include "SlottedPage.h"


uint16_t SlottedPage::insertData(char const* dataptr, uint16_t lenInBytes){
    Slot newSlot;
    if(lenInBytes <= sizeof(Slot) - 1) //we can store (sizeof(Slot) - 1) bytes directly
    {
        memcpy(&newSlot.data, dataptr, lenInBytes);
        newSlot.data.numBytes = lenInBytes;
        newSlot.data.isData = TRUE;
        newSlot.data.isIndirect = FALSE;

    } else { //store pointer to data in slot
        header.dataStart -= lenInBytes;
        memcpy( &data[header.dataStart], dataptr, lenInBytes);
        newSlot.dataPtr.offset = header.dataStart;
        newSlot.dataPtr.length = lenInBytes;
        newSlot.dataPtr.isData = FALSE;
        newSlot.dataPtr.isIndirect = FALSE;
    }
    slots[header.slotCount] = newSlot;
    return header.slotCount++; //first return, then increase!!
}

void SlottedPage::insertIndirection(uint16_t slotID, TID goalTID){
    slots[slotID].indirection.pageID = goalTID.pageID;
    slots[slotID].indirection.slotID = goalTID.slotID;
    slots[slotID].indirection.isIndirect = TRUE;
    slots[slotID].indirection.isData = FALSE;
}

Record SlottedPage::getRecordFromSlotID(uint16_t slotID){
    Slot s = slots[slotID];
    if(s.dataPtr.isData == FALSE) {
        return Record(s.dataPtr.length, &data[s.dataPtr.offset]);
    }
    else {
        return Record(s.data.numBytes, (char*)&s.data);
    }

}

Record SlottedPage::removeData(uint16_t slotID) {
    if(slots[slotID].dataPtr.isData == FALSE){
        header.fragmentedSpace += slots[slotID].dataPtr.length;
    }
    slots[slotID].indirection.isIndirect = TRUE;
    slots[slotID].indirection.isData = TRUE;
    //See definition of Slot TRUE/TRUE -> removed

    return getRecordFromSlotID(slotID); //TODO maybe change to void
}

void SlottedPage::removeIndirection(uint16_t slotID) {
    slots[slotID].indirection.isIndirect = TRUE;
    slots[slotID].indirection.isData = TRUE;
}


bool SlottedPage::hasEnoughSpace(uint16_t lenInBytes){
    uint16_t t = ((lenInBytes <= sizeof(Slot) - 1) ? (uint16_t)0 : lenInBytes); // accounts for possible storage directly in the Slot
    return getFreeSpaceInBytes() >= t + sizeof(Slot);
}

//Test if we can insert lenInBytes data
bool SlottedPage::hasEnoughSpaceAfterDefrag(uint16_t lenInBytes){
    uint16_t t = ((lenInBytes <= sizeof(Slot) - 1) ? (uint16_t)0 : lenInBytes); // accounts for possible storage directly in the Slot
    return getFreeSpaceInBytes() + header.fragmentedSpace >= t + sizeof(Slot);
}

uint16_t SlottedPage::getFreeSpaceInBytes(){
    return header.dataStart - header.slotCount * sizeof(Slot);
}

bool SlottedPage::hasValidData(uint16_t slotID) {
    //the slot must exist -> slotID < slotCount
    //the flag isIndirect must not be set
    return slotID < header.slotCount && slots[slotID].indirection.isIndirect == FALSE;
}

/*
 * Reorders data of not removed slots to get free space
 * Returns free space in bytes after defragmentation
 */
uint16_t SlottedPage::defrag(){
    SlottedPage spWorkingCopy;
    for(int i = 0; i < header.slotCount; ++i){
        Slot* s = &slots[i];
        if(s->dataPtr.isData == FALSE && s->dataPtr.isIndirect == FALSE){
            uint16_t wsID = spWorkingCopy.insertData(&data[s->dataPtr.offset], s->dataPtr.length);
            Slot ws = spWorkingCopy.slots[wsID];
            s->dataPtr.offset = ws.dataPtr.offset;
        }
    }
    //data is now sorted at the back -> update our SlottedPage
    header.dataStart = spWorkingCopy.header.dataStart;
    memcpy(&data[header.dataStart], &spWorkingCopy.data[spWorkingCopy.header.dataStart], PAGESIZE - sizeof(SPHeader) - spWorkingCopy.header.dataStart);
    header.fragmentedSpace = 0;
    return getFreeSpaceInBytes();
}