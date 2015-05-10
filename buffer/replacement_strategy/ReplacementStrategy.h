//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_REPLACEMENTSTRATEGY_H
#define PROJECT_REPLACEMENTSTRATEGY_H

#include <queue>

#include "../BufferFrame.h"


class ReplacementStrategy {
public:
    ReplacementStrategy() : fifoFirst(nullptr), fifoLast(nullptr), lruFirst(nullptr), lruLast(nullptr) {}

    void onUse(BufferFrame* bufferFrame); //the element was used -> update
    void onCreate(BufferFrame* bufferFrame); //the element was created -> insert it

    BufferFrame* popRemovable(); //pops removable candidate - or nullptr if we cannot remove anything (not the case for fifo)

private:

    void fifo_push_back(BufferFrame* bufferFrame);
    void fifo_remove(BufferFrame* bufferFrame);

    BufferFrame* fifoFirst;
    BufferFrame* fifoLast;

    void lru_push_back(BufferFrame* bufferFrame);
    void lru_remove(BufferFrame* bufferFrame);

    BufferFrame* lruFirst;
    BufferFrame* lruLast;

    BufferFrame* fifo_pop_clean();
    BufferFrame* fifo_pop_unfixed();
    BufferFrame* lru_pop_unfixed();

};

#endif //PROJECT_REPLACEMENTSTRATEGY_H
