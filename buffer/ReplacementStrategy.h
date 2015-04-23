//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_REPLACEMENTSTRATEGY_H
#define PROJECT_REPLACEMENTSTRATEGY_H

#include <queue>

#include "BufferFrameWrapper.h"


/*
 * only fifo strategy
 * TODO: change strategy
 */
class ReplacementStrategy {
public:
    void update(BufferFrameWrapper &bufferFrameWrapper); //the element was used -> update
    void create(BufferFrameWrapper &bufferFrameWrapper); //the element was created -> insert it

    BufferFrameWrapper pop(); //pops removable candidate - or nothing if we cannot remove anything (not the case for fifo)

private:
    std::queue<BufferFrameWrapper> fifo;
};

#endif //PROJECT_REPLACEMENTSTRATEGY_H
