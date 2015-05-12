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

//4 byte Slot
struct Slot {
    uint32_t controlBits; //TODO
    uint16_t offset;
    uint16_t length;
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
    uint16_t insert(char const* dataptr, uint16_t lenInBytes);

    /*
     * Try to insert data directly into the slot
     */
    bool tryUpdate(uint16_t slotID, char const* dataptr, uint16_t lenInBytes);

    void insertIndirection(uint16_t slotID, TID indirection);

    /*
        * Only use this method, when you are sure, that you can remove it
        * e.g. it must exist and is no indirection (hasValidData)
        */
    void remove(uint16_t slotID);

    Record &getRecordFromSlotID(uint16_t slotID);


    //Test if we can insert lenInBytes data
    bool hasEnoughSpace(uint16_t lenInBytes);

    //Test if we can insert lenInBytes data
    bool hasEnoughSpaceAfterDefrag(uint16_t lenInBytes);

    uint16_t getFreeSpaceInBytes();
    uint16_t getFreeSpaceInBytesAfterDefrag();

    uint16_t getLenBytes(uint16_t slotID);
    TID getIndirection(uint16_t slotID);

    bool isIndirection(uint16_t slotID);
    bool isRemoved(uint16_t slotID);
    bool isValid(uint16_t slotID); //slotID < slotCount
    /*
     * Reorders data of not removed slots to get free space
     * Returns free space in bytes after defragmentation
     */
    uint16_t defrag();
};

#endif //PROJECT_SLOTTEDPAGE_H