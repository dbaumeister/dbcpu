//
// Created by dbaumeister on 12.05.15.
//

#ifndef PROJECT_TID_H
#define PROJECT_TID_H

#include <stdint.h>

//8 byte TID
struct TID {
    uint16_t slotID : 16;
    uint64_t pageID : 48;
};

#endif //PROJECT_TID_H
