//
// Created by dbaumeister on 22.04.15.
//

#include "BufferFrameWrapper.h"

uint64_t BufferFrameWrapper::getID() {
    return bufferFrame.getID();
}

uint16_t BufferFrameWrapper::getSegmentID() {
    return bufferFrame.getID() >> 48;
}

uint64_t BufferFrameWrapper::getPageID() {
    uint64_t pageID = 1;
    return bufferFrame.getID() & ((pageID << 48) - 1);
}

BufferFrame BufferFrameWrapper::getBufferFrame() {
    return bufferFrame;
}

void BufferFrameWrapper::setBufferFrame(BufferFrame frame) {
    bufferFrame = frame;
}
