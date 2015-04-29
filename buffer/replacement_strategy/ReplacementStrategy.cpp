//
// Created by dbaumeister on 22.04.15.
//

#include "ReplacementStrategy.h"

void ReplacementStrategy::onCreate(BufferFrame* bufferFrame) {
    fifo.push(bufferFrame);
    onUse(bufferFrame);
}

void ReplacementStrategy::onUse(BufferFrame* bufferFrame) {
}

BufferFrame* ReplacementStrategy::popRemovable() {
    //The fifo queue is not empty, when we reach this point in our program.
    BufferFrame* bufferFrame = fifo.front();
    if(bufferFrame->isExclusive()) {
        throw NO_REMOVABLE_FRAME_ERROR;
    }

    fifo.pop();
    return bufferFrame;
}
