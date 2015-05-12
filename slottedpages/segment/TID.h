//
// Created by dbaumeister on 12.05.15.
//

#ifndef PROJECT_TID_H
#define PROJECT_TID_H

#include <stdint.h>

//8 byte TID
struct TID {
    //Don't use all 32 bits: we need the last 2 bits in case of indirection
    uint16_t slotID : 16;
    uint64_t pageID : 46;
    //2 bit padding
};

#endif //PROJECT_TID_H
