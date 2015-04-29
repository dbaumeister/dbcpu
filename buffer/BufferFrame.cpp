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

uint16_t BufferFrame::getSegmentID() {
    uint64_t rawID = id;
    return rawID >> 48; //overflow is intended
}

uint64_t BufferFrame::getPageID() {
    uint64_t rawID = id;
    return rawID & (((uint64_t) 1 << 48) - 1);
}

bool BufferFrame::isExclusive() {
    return exclusive;
}

void BufferFrame::setExclusive(bool isExclusive) {
    exclusive = isExclusive;
}

bool BufferFrame::isDirty() {
    return dirty;
}

void BufferFrame::setDirty(bool isDirty) {
    dirty = isDirty;
}

void BufferFrame::setState(uint16_t state) {
    this->state = state;
}

void BufferFrame::clearControlDataAndSetID(uint64_t id) {
    setState(0);
    setDirty(false);
    setExclusive(false);
    this->id = id;
}
