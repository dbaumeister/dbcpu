//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_BUFFERFRAMEWRAPPER_H
#define PROJECT_BUFFERFRAMEWRAPPER_H


#include "BufferFrame.h"

#define STATE_NEW 1

/*
 * Provides BufferManager with additional information about BufferFrame
 * Hides control informations from actors outside of the buffer system
 */
class BufferFrameWrapper {
public:
    BufferFrameWrapper(uint16_t state, BufferFrame& bufferFrame)
    : state(state), bufferFrame(bufferFrame) {}

    BufferFrameWrapper(uint16_t state, uint64_t id, void* data)
            : state(state), bufferFrame(BufferFrame(id, data)) {}

    uint64_t getID();
    uint16_t getSegmentID();
    uint64_t getPageID();

    bool isDirty();
    bool isClean();

    void setState(uint16_t newState);

    BufferFrame getBufferFrame();
    void setBufferFrame(BufferFrame frame);

private:
    //TODO: Latch (mutex)
    uint16_t state; //State (e.g. dirty)
    BufferFrame bufferFrame;
};

#endif //PROJECT_BUFFERFRAMEWRAPPER_H
