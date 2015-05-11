//
// Created by dbaumeister on 05.05.15.
//

#ifndef PROJECT_SPSEGMENT_H
#define PROJECT_SPSEGMENT_H

#include <stdint.h>
#include <iostream>
#include "../../buffer/BufferManager.h"
#include "../../buffer/BufferFrame.h"
#include "../Record.h"

const uint8_t FALSE = 0;
const uint8_t TRUE = 1;

//8 byte TID
struct TID {
    //Don't use all 32 bits: we need the last 2 bits in case of indirection
    uint16_t slotID : 16;
    uint64_t pageID : 46;
    //2 bit padding
};

//8 byte Slot
struct Slot {
    struct Indirection { //-> Indirect TRUE - Data FALSE
        uint16_t slotID : 16;
        uint64_t pageID : 46;
        uint8_t isIndirect : 1;
        uint8_t isData : 1;
    };
    struct DataPtr { //-> Indirect FALSE - Data FALSE
        uint32_t length : 32;
        uint32_t offset : 30;
        uint8_t isIndirect : 1;
        uint8_t isData : 1;
    };
    struct Data { //-> Indirect FALSE - Data TRUE
        uint64_t data : 56;
        uint8_t numBytes : 6;
        uint8_t isIndirect : 1;
        uint8_t isData : 1;
    };

    //When removed: Indirect TRUE && Data TRUE!!

    union {
        //The last two bits are the same
        // -> check with isRedirect == TRUE/FALSE
        // -> check with isData == TRUE/FALSE
        Indirection indirection;
        DataPtr dataPtr;
        Data data;
    };

};

//PAGESIZE SlottedPage
struct SlottedPage {
    struct SPHeader {
        uint16_t slotCount = 0; //is also a pointer to the first free slot
        uint16_t dataStart = PAGESIZE - sizeof(SPHeader);
        uint16_t fragmentedSpace = 0;
    } header;
    union {
        Slot slots[(PAGESIZE - sizeof(SPHeader)) / sizeof(Slot)];
        char data[PAGESIZE - sizeof(SPHeader)];
    };

    /*
     * returns slot ID of insert
     * Only use this method when you are sure, that you can insert!
     * (e.g. hasEnoughSpace == true)
     */
    uint16_t insertData(void* dataptr, uint16_t lenInBytes){
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

    /*
     * insert an indirection to goalTID into the slotID (which must exist beforehand)
     */
    void insertIndirection(uint16_t slotID, TID goalTID){
        slots[slotID].indirection.pageID = goalTID.pageID;
        slots[slotID].indirection.slotID = goalTID.slotID;
        slots[slotID].indirection.isIndirect = TRUE;
        slots[slotID].indirection.isData = FALSE;
    }

    Record getRecordFromSlotID(uint16_t slotID){
        Slot s = slots[slotID];
        if(s.dataPtr.isData == FALSE) {
            return Record(s.dataPtr.length, &data[s.dataPtr.offset]);
        }
        else {
            return Record(s.data.numBytes, (char*)&s.data);
        }

    }

    /*
     * Only use this method, when you are sure, that you can remove it
     * e.g. it must exist and is no indirection (hasValidData)
     */
    Record removeData(uint16_t slotID) {
        if(slots[slotID].dataPtr.isData == FALSE){
            header.fragmentedSpace += slots[slotID].dataPtr.length;
        }
        slots[slotID].indirection.isIndirect = TRUE;
        slots[slotID].indirection.isData = TRUE;
        //See definition of Slot TRUE/TRUE -> removed

        return getRecordFromSlotID(slotID); //TODO maybe change to void
    }

    /*
     * TODO See if this function is needed at all
     */
    void removeIndirection(uint16_t slotID) {
        slots[slotID].indirection.isIndirect = TRUE;
        slots[slotID].indirection.isData = TRUE;
    }

    //Test if we can insert lenInBytes data
    bool hasEnoughSpace(uint16_t lenInBytes){
        uint16_t t = ((lenInBytes <= sizeof(Slot) - 1) ? (uint16_t)0 : lenInBytes); // accounts for possible storage directly in the Slot
        return getFreeSpaceInBytes() >= t + sizeof(Slot);
    }

    uint16_t getFreeSpaceInBytes(){
        return header.dataStart - header.slotCount * sizeof(Slot);
    }

    bool hasValidData(uint16_t slotID) {
        //the slot must exist -> slotID < slotCount
        //the flag isIndirect must not be set
        return slotID < header.slotCount && slots[slotID].indirection.isIndirect == FALSE;
    }

    /*
     * Reorders data of not removed slots to get free space
     * Returns free space in bytes after defragmentation
     */
    uint16_t defrag(){
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
};

class SPSegment {
public:
    SPSegment(BufferManager& bm) : bufferManager(bm) {}

    TID insert(Record& record);
    bool remove(TID tid);
    bool update(TID tid, Record& record);
    Record& lookup(TID tid);

    //~SPSegment() {
        //TODO unfix all pages
    //}

private:
    BufferManager& bufferManager;

    std::unordered_map<uint64_t, SlottedPage*> slottedPageMap; //stores all Slotted Pages for fast lookups (uint64_t => pageID)

    std::vector<std::pair<uint64_t, SlottedPage*>> slottedPages; //stores all Slotted Pages -
    // > search through this list to find pages with sufficient space

    std::vector<SlottedPage*> highlyFragmentedPages; //When inserting -> search through this list before creating a new
};

#endif //PROJECT_SPSEGMENT_H
