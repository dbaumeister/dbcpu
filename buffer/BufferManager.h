//
// Created by dbaumeister on 21.04.15.
//

#ifndef PROJECT_BUFFERMANAGER_H
#define PROJECT_BUFFERMANAGER_H

#include <stdint.h>

#include "BufferFrame.h"
#include "replacement_strategies/ReplacementStrategy.h"
#include "../collection/LockedCollection.h"
#include "../exceptions/exceptions.h"


#define DATA_PATH_PREFIX "data"

class BufferManager{
public:
    BufferManager(uint64_t maxPageCount) : pageCountMax(maxPageCount), pageCount(0) {}

    BufferFrame* fixPage(uint64_t id, bool exclusive);
    void unfixPage(BufferFrame* frame, bool isDirty);

    ~BufferManager();

private:
    uint64_t pageCountMax;
    uint64_t pageCount;
    LockedCollection<uint64_t , BufferFrame*> collection;
    ReplacementStrategy replacementStrategy;

    BufferFrame* createBufferFrame(uint64_t id);
    BufferFrame* recreateBufferFrame(uint64_t id, BufferFrame* bufferFrame);

    void writeToDisk(BufferFrame* frame); //low level function
    void readFromDisk(BufferFrame* frame); //low level function, assumes that data is already allocated
};

#endif //PROJECT_BUFFERMANAGER_H
