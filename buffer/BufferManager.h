//
// Created by dbaumeister on 21.04.15.
//

#ifndef PROJECT_BUFFERMANAGER_H
#define PROJECT_BUFFERMANAGER_H

#include <stdint.h>
//#include <unordered_map> //requires -std=c++11

#include "BufferFrame.h"
#include "BufferFrameWrapper.h"
#include "ReplacementStrategy.h"
#include "../collection/LockedCollection.h"


#define IO_ERROR -2

class BufferManager{
public:
    BufferManager(uint64_t maxPageCount) : maxPageCount(maxPageCount), pageCount(0) {}

    BufferFrame fixPage(uint64_t id, bool exclusive);
    void unfixPage(BufferFrame& frame, bool isDirty);

    ~BufferManager();

private:
    uint64_t maxPageCount;
    uint64_t pageCount;
    LockedCollection<uint64_t , BufferFrameWrapper> collection;
    ReplacementStrategy replacementStrategy;

    BufferFrameWrapper createBufferFrame(uint64_t id);
    BufferFrameWrapper recreateBufferFrame(uint64_t id, BufferFrameWrapper &bufferFrameWrapper);

    void writeToDisk(BufferFrameWrapper& frame); //low level function
    void readFromDisk(BufferFrameWrapper& frame); //low level function, assumes that data is allready allocated
};

#endif //PROJECT_BUFFERMANAGER_H
