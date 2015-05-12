//
// Created by dbaumeister on 12.05.15.
//

#ifndef PROJECT_SLOTTEDPAGE_H
#define PROJECT_SLOTTEDPAGE_H

#include <stdint.h>
#include <string.h>
#include "../../buffer/BufferFrame.h"
#include "../Record.h"
#include "TID.h"


const uint8_t FALSE = 0;
const uint8_t TRUE = 1;

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
    uint16_t insertData(char const* dataptr, uint16_t lenInBytes);

    /*
     * insert an indirection to goalTID into the slotID (which must exist beforehand)
     */
    void insertIndirection(uint16_t slotID, TID goalTID);

    Record getRecordFromSlotID(uint16_t slotID);

    /*
     * Only use this method, when you are sure, that you can remove it
     * e.g. it must exist and is no indirection (hasValidData)
     */
    Record removeData(uint16_t slotID);

    /*
     * TODO See if this function is needed at all
     */
    void removeIndirection(uint16_t slotID);

    //Test if we can insert lenInBytes data
    bool hasEnoughSpace(uint16_t lenInBytes);

    //Test if we can insert lenInBytes data
    bool hasEnoughSpaceAfterDefrag(uint16_t lenInBytes);

    uint16_t getFreeSpaceInBytes();

    bool hasValidData(uint16_t slotID);

    /*
     * Reorders data of not removed slots to get free space
     * Returns free space in bytes after defragmentation
     */
    uint16_t defrag();
};

#endif //PROJECT_SLOTTEDPAGE_H
