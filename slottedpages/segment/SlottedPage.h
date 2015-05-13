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

const uint8_t BM_DEFAULT = ~(uint8_t)0;
const uint8_t BM_REMOVED = ~(uint8_t)0 - (uint8_t)1;


struct Slot {
    uint8_t controlBits;
    uint32_t padding : 24;
    uint16_t offset;
    uint16_t length;
};

//PAGESIZE SlottedPage
struct SlottedPage {
    struct SPHeader {
        uint16_t slotCount = 0; //is also a pointer to the first free slot
        uint16_t dataStart = PAGESIZE - sizeof(SPHeader);
        uint16_t fragmentedSpace = 0;
        uint16_t numUnusedSlots = 0;
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
    uint16_t insertNewSlot(char const* dataptr, uint16_t lenInBytes);

    /*
     * Try to insert data directly into the slot
     */
    bool tryUpdate(uint16_t slotID, char const* dataptr, uint16_t lenInBytes);
    bool tryUpdateSlotWithIndirection(uint16_t slotID, char const* dataptr, uint16_t lenInBytes);
    bool tryUpdateRemovedSlot(uint16_t slotID, char const* dataptr, uint16_t lenInBytes);

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
    uint16_t getNumUnusedSlots();
    uint16_t getFirstUnusedSlot();

    TID getIndirection(uint16_t slotID);

    bool isIndirection(uint16_t slotID);
    bool isRemoved(uint16_t slotID);
    bool isValid(uint16_t slotID); //slotID < slotCount

    /*
     * Reorders data of not removed slots to get free space
     * Returns free space in bytes after defragmentation
     */
    uint16_t defrag();


private:
    void setControlbitsToRemoved(uint16_t slotID);
    void setControlbitsToDefault(uint16_t slotID);

};

#endif //PROJECT_SLOTTEDPAGE_H
