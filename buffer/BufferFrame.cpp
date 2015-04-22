//
// Created by dbaumeister on 21.04.15.
//

#include "BufferFrame.h"

void *BufferFrame::getData() {
    return data;
}

uint64_t BufferFrame::getID() {
    return id;
}
