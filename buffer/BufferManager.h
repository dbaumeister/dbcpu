//
// Created by dbaumeister on 21.04.15.
//

#ifndef PROJECT_BUFFERMANAGER_H
#define PROJECT_BUFFERMANAGER_H

#include <stdint.h>

#include "BufferFrame.h"
#include "replacement_strategy/ReplacementStrategy.h"
#include "../collection/LockedCollection.h"
#include "../exceptions/exceptions.h"
#include "disk_io/DiskIO.h"


class BufferManager{
public:
    BufferManager(uint64_t maxPageCount) : pageCountMax(maxPageCount), pageCount(0) {}

    BufferFrame* fixPage(uint64_t id, bool isExclusive);
    void unfixPage(BufferFrame* frame, bool isDirty);

    ~BufferManager();

private:
    uint64_t pageCountMax;
    uint64_t pageCount;
    LockedCollection<uint64_t , BufferFrame*> collection;
    ReplacementStrategy replacementStrategy;
    DiskIO io;

    void setExclusiveLock(BufferFrame* bufferFrame, bool isExclusive);

    BufferFrame* createBufferFrame(uint64_t id);
    BufferFrame* recreateBufferFrame(uint64_t id, BufferFrame* bufferFrame);

};

#endif //PROJECT_BUFFERMANAGER_H
