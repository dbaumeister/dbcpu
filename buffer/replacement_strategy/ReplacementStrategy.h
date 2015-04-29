//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_REPLACEMENTSTRATEGY_H
#define PROJECT_REPLACEMENTSTRATEGY_H

#include <queue>

#include "../BufferFrame.h"
#include "../../exceptions/exceptions.h"


/*
 * only fifo strategy
 * TODO: change strategy
 */
class ReplacementStrategy {
public:
    void onUse(BufferFrame* bufferFrameWrapper); //the element was used -> update
    void onCreate(BufferFrame* bufferFrameWrapper); //the element was created -> insert it

    BufferFrame* popRemovable(); //pops removable candidate - or nothing if we cannot remove anything (not the case for fifo)

private:
    std::queue<BufferFrame*> fifo;
};

#endif //PROJECT_REPLACEMENTSTRATEGY_H