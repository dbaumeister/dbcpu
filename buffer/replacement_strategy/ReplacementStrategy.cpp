//
// Created by dbaumeister on 22.04.15.
//

#include "ReplacementStrategy.h"

/*
 * Always push back to fifo
 */
void ReplacementStrategy::onCreate(BufferFrame* bufferFrame) {
    fifo.push_back(bufferFrame);
}

/*Prio:
 * 1. check if in fifo
 * 2. only put dirty pages from fifo to lru
 * 3. don't change the position of clean pages in fifo
 * 4. check if in lru
 * 5. remove from lru and push to back in lru
 */
void ReplacementStrategy::onUse(BufferFrame* bufferFrame) {
    if(fifo.contains(bufferFrame) && bufferFrame->isDirty()){
        fifo.remove(bufferFrame);
        lru.push_back(bufferFrame);
    }
    else if(lru.contains(bufferFrame)) {
        lru.remove(bufferFrame);
        lru.push_back(bufferFrame);
    }
}

/*Prio:
 * 0. Only remove unfixed pages! return nullptr otherwise
 * 1. take first clean page from fifo
 * 2. take first dirty page from fifo
 * 3. take first page from lru (only contains dirty pages)
 */
BufferFrame* ReplacementStrategy::popRemovable() {
    BufferFrame* bufferFrame = fifo.pop_clean();
    if(bufferFrame == nullptr){
        bufferFrame = fifo.pop_unfixed();
        if(bufferFrame == nullptr) {
            bufferFrame = lru.pop_unfixed();
        }
    }
    return bufferFrame;
}
