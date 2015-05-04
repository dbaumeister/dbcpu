//
// Created by dbaumeister on 29.04.15.
//

#ifndef PROJECT_DISKIO_H
#define PROJECT_DISKIO_H

#include <unordered_map>
#include <stdint.h>
#include "../BufferFrame.h"


#define DATA_PATH_PREFIX ""

class DiskIO {
public:
    void writeToDisk(BufferFrame* frame);
    void readFromDisk(BufferFrame* frame); //assumes that data is already allocated

    void closeFiles();

private:
    std::unordered_map<uint16_t, int> filedescriptors;
    int getOpenFileDescriptor(uint16_t segID);
    int getFileDescriptor(BufferFrame* bufferFrame);
};

#endif //PROJECT_DISKIO_H
