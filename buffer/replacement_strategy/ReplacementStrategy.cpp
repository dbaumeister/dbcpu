//
// Created by dbaumeister on 22.04.15.
//

#include "ReplacementStrategy.h"

/*
 * Always push back to fifo
 */
void ReplacementStrategy::onCreate(BufferFrame* bufferFrame) {
    fifo_push_back(bufferFrame);
}

/*Prio:
 * 1. check if in fifo
 * 2. only put dirty pages from fifo to lru
 * 3. don't change the position of clean pages in fifo
 * 4. check if in lru
 * 5. remove from lru and push to back in lru
 */
void ReplacementStrategy::onUse(BufferFrame* bufferFrame) {
    if(bufferFrame->isInFifo()) {
        if(!bufferFrame->isDirty()){
            fifo_remove(bufferFrame);
            lru_push_back(bufferFrame);
        }
    }
    else {
        lru_remove(bufferFrame);
        lru_push_back(bufferFrame);
    }
}

/*Prio:
 * 0. Only remove unfixed pages! return nullptr otherwise
 * 1. take first clean page from fifo
 * 2. take first dirty page from fifo
 * 3. take first page from lru (only contains dirty pages)
 */
BufferFrame* ReplacementStrategy::popRemovable() {
    BufferFrame* bufferFrame = fifo_pop_clean();
    if(bufferFrame == nullptr){
        bufferFrame = fifo_pop_unfixed();
        if(bufferFrame == nullptr) {
            bufferFrame = lru_pop_unfixed();
        }
    }
    return bufferFrame;
}

void ReplacementStrategy::fifo_push_back(BufferFrame *bufferFrame) {
    if(fifoLast == nullptr){
        fifoFirst = bufferFrame;
        bufferFrame->prev = nullptr;
        bufferFrame->next = nullptr;
    }
    else {
        bufferFrame->next = nullptr;
        bufferFrame->prev = fifoLast;
        fifoLast->next = bufferFrame;
    }

    fifoLast = bufferFrame;
    bufferFrame->setInFifo(true);
}

void ReplacementStrategy::lru_push_back(BufferFrame *bufferFrame) {
    if(lruLast == nullptr){
        lruFirst = bufferFrame;
        bufferFrame->prev = nullptr;
        bufferFrame->next = nullptr;
    }
    else {
        bufferFrame->next = nullptr;
        bufferFrame->prev = lruLast;
        lruLast->next = bufferFrame;
    }

    lruLast = bufferFrame;
    bufferFrame->setInFifo(false);
}

void ReplacementStrategy::fifo_remove(BufferFrame *bufferFrame) {
    //Case 1: e is first and last
    if(bufferFrame == fifoFirst && bufferFrame == fifoLast) {
        fifoFirst = nullptr;
        fifoLast = nullptr;
    } else if(bufferFrame == fifoFirst){
        fifoFirst = fifoFirst->next;
        fifoFirst->prev = nullptr;
    } else if(bufferFrame == fifoLast){
        fifoLast = fifoLast->prev;
        fifoLast->next = nullptr;
    } else {
        bufferFrame->prev->next = bufferFrame->next;
        bufferFrame->next->prev = bufferFrame->prev;
    }
    bufferFrame->next = nullptr;
    bufferFrame->prev = nullptr;
}

void ReplacementStrategy::lru_remove(BufferFrame *bufferFrame) {
    //Case 1: e is first and last
    if(bufferFrame == lruFirst && bufferFrame == lruLast) {
        lruFirst = nullptr;
        lruLast = nullptr;
    } else if(bufferFrame == lruFirst){
        lruFirst = lruFirst->next;
        lruFirst->prev = nullptr;
    } else if(bufferFrame == lruLast){
        lruLast = lruLast->prev;
        lruLast->next = nullptr;
    } else {
        bufferFrame->prev->next = bufferFrame->next;
        bufferFrame->next->prev = bufferFrame->prev;
    }
    bufferFrame->next = nullptr;
    bufferFrame->prev = nullptr;
}

BufferFrame *ReplacementStrategy::fifo_pop_clean() {

    BufferFrame* e = fifoFirst;

    //skip as long as e is not null and contains a dirty BufferFrame or one that has users
    while(e != nullptr && (e->isDirty() || e->getUserCount() > 0)){
        e = e->next;
    }
    //-> e is either nullptr or contains a clean unused BufferFrame
    if(e == nullptr) return nullptr;

    if(e == fifoFirst && e == fifoLast) {
        fifoFirst = nullptr;
        fifoLast = nullptr;
    } else if(e == fifoFirst){
        fifoFirst = fifoFirst->next;
        fifoFirst->prev = nullptr;
    } else if(e == fifoLast){
        fifoLast = fifoLast->prev;
        fifoLast->next = nullptr;
    } else {
        e->prev->next = e->next;
        e->next->prev = e->prev;
    }

    e->next = nullptr;
    e->prev = nullptr;
    return e;
}

BufferFrame *ReplacementStrategy::lru_pop_unfixed() {

    BufferFrame* e = lruFirst;

    //skip as long as e is not null and contains a dirty BufferFrame or one that has users
    while(e != nullptr && e->getUserCount() > 0){
        e = e->next;
    }
    //-> e is either nullptr or contains a clean unused BufferFrame
    if(e == nullptr) return nullptr;

    if(e == lruFirst && e == lruLast) {
        lruFirst = nullptr;
        lruLast = nullptr;
    } else if(e == lruFirst){
        lruFirst = lruFirst->next;
        lruFirst->prev = nullptr;
    } else if(e == lruLast){
        lruLast = lruLast->prev;
        lruLast->next = nullptr;
    } else {
        e->prev->next = e->next;
        e->next->prev = e->prev;
    }

    e->next = nullptr;
    e->prev = nullptr;
    return e;
}

BufferFrame *ReplacementStrategy::fifo_pop_unfixed() {

    BufferFrame* e = fifoFirst;

    //skip as long as e is not null and contains a dirty BufferFrame or one that has users
    while(e != nullptr && e->getUserCount() > 0){
        e = e->next;
    }
    //-> e is either nullptr or contains a clean unused BufferFrame
    if(e == nullptr) return nullptr;

    if(e == fifoFirst && e == fifoLast) {
        fifoFirst = nullptr;
        fifoLast = nullptr;
    } else if(e == fifoFirst){
        fifoFirst = fifoFirst->next;
        fifoFirst->prev = nullptr;
    } else if(e == fifoLast){
        fifoLast = fifoLast->prev;
        fifoLast->next = nullptr;
    } else {
        e->prev->next = e->next;
        e->next->prev = e->prev;
    }

    e->next = nullptr;
    e->prev = nullptr;
    return e;
}
