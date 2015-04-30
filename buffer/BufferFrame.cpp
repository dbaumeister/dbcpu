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


bool BufferFrame::isDirty() {
    return dirty;
}

void BufferFrame::setDirty(bool isDirty) {
    dirty = isDirty;
}


void BufferFrame::clearControlDataAndSetID(uint64_t id) {
    setDirty(false);
    this->id = id;
}

void BufferFrame::lockRead() {
    pthread_rwlock_rdlock(&frame_rwlock);
}

void BufferFrame::lockWrite() {
    pthread_rwlock_wrlock(&frame_rwlock);
}

void BufferFrame::unlockFrame() {
    pthread_rwlock_unlock(&frame_rwlock);
}

void BufferFrame::lockFrame(bool isExclusive) {
    if(isExclusive) {
        lockWrite();
    }
    else {
        lockRead();
    }
}

void BufferFrame::fix() {
    ++user_count;
}

void BufferFrame::unfix() {
    --user_count;
}

unsigned int BufferFrame::getUserCount() {
    return user_count;
}
