//
// Created by dbaumeister on 11.05.15.
//

#ifndef PROJECT_METASEGMENT_H
#define PROJECT_METASEGMENT_H

#include "../../buffer/BufferManager.h"

class MetaSegment {
public:
    MetaSegment(BufferManager& bm) : bufferManager(bm) {}
private:
    BufferManager& bufferManager;
};

#endif //PROJECT_METASEGMENT_H
