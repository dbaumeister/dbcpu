//
// Created by dbaumeister on 22.04.15.
//

#include "ReplacementStrategy.h"

void ReplacementStrategy::create(BufferFrameWrapper bufferFrameWrapper) {
    fifo.push(bufferFrameWrapper);
    update(bufferFrameWrapper);
}

void ReplacementStrategy::update(BufferFrameWrapper bufferFrameWrapper) {
}

BufferFrameWrapper ReplacementStrategy::pop() {
    //The fifo queue is not empty, when we reach this point in our program.
    BufferFrameWrapper bufferFrameWrapper = fifo.front();
    fifo.pop();
    return bufferFrameWrapper;
}
