//
// Created by dbaumeister on 22.04.15.
//

#include <iostream>

#include "BufferFrameWrapper.h"

uint64_t BufferFrameWrapper::getID() {
    return bufferFrame.getID();
}

uint16_t BufferFrameWrapper::getSegmentID() {
    uint64_t rawID = bufferFrame.getID();
    return rawID >> 48; //overflow is intended
}

uint64_t BufferFrameWrapper::getPageID() {
    uint64_t rawID = bufferFrame.getID();
    return rawID & (((uint64_t) 1 << 48) - 1);
}

BufferFrame BufferFrameWrapper::getBufferFrame() {
    return bufferFrame;
}

bool BufferFrameWrapper::isExclusive() {
    return exclusive;
}

void BufferFrameWrapper::setExclusive(bool isExclusive) {
    exclusive = isExclusive;
}

bool BufferFrameWrapper::isDirty() {
    return dirty;
}

void BufferFrameWrapper::setDirty(bool isDirty) {
    dirty = isDirty;
}
