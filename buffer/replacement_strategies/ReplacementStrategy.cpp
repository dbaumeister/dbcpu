//
// Created by dbaumeister on 22.04.15.
//

#include "ReplacementStrategy.h"

void ReplacementStrategy::onCreate(BufferFrameWrapper* bufferFrameWrapper) {
    fifo.push(bufferFrameWrapper);
    onUse(bufferFrameWrapper);
}

void ReplacementStrategy::onUse(BufferFrameWrapper* bufferFrameWrapper) {
}

BufferFrameWrapper* ReplacementStrategy::popRemovable() {
    //The fifo queue is not empty, when we reach this point in our program.
    BufferFrameWrapper* bufferFrameWrapper = fifo.front();
    if(bufferFrameWrapper->isExclusive()) {
        throw NO_REMOVABLE_FRAME_ERROR;
    }

    fifo.pop();
    return bufferFrameWrapper;
}
