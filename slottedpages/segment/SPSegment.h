//
// Created by dbaumeister on 05.05.15.
//

#ifndef PROJECT_SPSEGMENT_H
#define PROJECT_SPSEGMENT_H

#include <stdint.h>
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
        uint64_t data : 62;
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
        uint16_t slotCount = 0; //equals a pointer to the first free slot
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
    uint16_t insert(void* dataptr, uint16_t lenInBytes){
        Slot newSlot;
        if(lenInBytes <= sizeof(Slot) - 1) //we can store 3 bytes directly
        {
            memcpy(&newSlot.data, dataptr, lenInBytes);
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
     * Only use this method, when you are sure, that you can remove it
     * e.g. it must exist and is no indirection (hasValidData)
     */
    bool remove(uint16_t slotID) {
        if(slots[slotID].dataPtr.isData == FALSE){
            header.fragmentedSpace += slots[slotID].dataPtr.length;
            //TODO consider setting length and offset to 0?
        }
        slots[slotID].indirection.isIndirect = TRUE;
        slots[slotID].indirection.isData = TRUE;
        //See definition of Slot TRUE/TRUE -> removed

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
};

class SPSegment {
public:
    TID insert(Record& record);
    bool remove(TID tid);
    bool update(TID tid, Record& record);
    Record& lookup(TID tid);

    //~SPSegment() {
        //TODO unfix all pages
    //}

private:
    std::unordered_map<uint64_t, SlottedPage*> slottedPageMap; //stores all Slotted Pages for fast lookups (uint64_t => pageID)

    std::vector<std::pair<uint64_t, SlottedPage*>> slottedPages; //stores all Slotted Pages -
    // > search through this list to find pages with sufficient space

    std::vector<SlottedPage*> highlyFragmentedPages; //When inserting -> search through this list before creating a new
};

#endif //PROJECT_SPSEGMENT_H
