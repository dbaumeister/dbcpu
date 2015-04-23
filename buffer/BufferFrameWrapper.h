//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_BUFFERFRAMEWRAPPER_H
#define PROJECT_BUFFERFRAMEWRAPPER_H


#include "BufferFrame.h"

#define STATE_NEW 1

/*
 * Provides BufferManager with additional information about BufferFrame
 * Hides control information from actors outside of the buffer system
 */
class BufferFrameWrapper {
public:
    BufferFrameWrapper(BufferFrame& bufferFrame)
    : state(0), bufferFrame(bufferFrame), dirty(false) {}

    BufferFrameWrapper(uint64_t id, void* data)
            : state(0), bufferFrame(BufferFrame(id, data)), dirty(false) {}

    uint64_t getID();
    uint16_t getSegmentID();
    uint64_t getPageID();

    bool isDirty();
    void setDirty(bool isDirty);

    BufferFrame getBufferFrame();
    void setBufferFrame(BufferFrame &frame);

private:
    //TODO: Latch (mutex)
    uint16_t state; //State for replacement Strategy
    BufferFrame bufferFrame;
    bool dirty;
};

#endif //PROJECT_BUFFERFRAMEWRAPPER_H
