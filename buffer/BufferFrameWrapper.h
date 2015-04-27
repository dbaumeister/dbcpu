//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_BUFFERFRAMEWRAPPER_H
#define PROJECT_BUFFERFRAMEWRAPPER_H


#include "BufferFrame.h"


/*
 * Provides BufferManager with additional information about BufferFrame
 * Hides control information from actors outside of the buffer system
 */
class BufferFrameWrapper {
public:

    BufferFrameWrapper(uint64_t id, void* data)
            : state(0), bufferFrame(BufferFrame(id, data)), dirty(false), exclusive(false) {}

    uint64_t getID();
    uint16_t getSegmentID();
    uint64_t getPageID();

    bool isDirty();
    void setDirty(bool isDirty);

    bool isExclusive();
    void setExclusive(bool isExclusive);

    BufferFrame getBufferFrame();

private:
    //TODO: Latch (mutex)
    uint16_t state; //State for replacement Strategy
    BufferFrame bufferFrame;
    bool dirty, exclusive;
};

#endif //PROJECT_BUFFERFRAMEWRAPPER_H
