//
// Created by dbaumeister on 29.04.15.
//


#include <sys/fcntl.h>
#include <unistd.h>
#include <string.h>

#include "DiskIO.h"


void DiskIO::writeToDisk(BufferFrame* bufferFrame) {

    //printf("Write to disk: Segment %d - Page %lu\n", bufferFrame->getSegmentID(), bufferFrame->getPageID());
    //persists the frame data
    int fd = getFileDescriptor(bufferFrame);

    if(pwrite(fd, bufferFrame->getData(), PAGESIZE, bufferFrame->getPageID() * PAGESIZE) == -1) {
        fprintf(stderr, "Could not write in data file: %s.\n", strerror(errno));
        exit(1);
    }
}

void DiskIO::readFromDisk(BufferFrame* bufferFrame) {
    //fill frames data
    //Get BufferedFrames by reading from disk at id, create it when it does not exist

    //printf("Read from disk: Segment %d - Page %lu\n", bufferFrame->getSegmentID(), bufferFrame->getPageID());
    int fd = getFileDescriptor(bufferFrame);

    if(pread(fd, bufferFrame->getData(), PAGESIZE, bufferFrame->getPageID() * PAGESIZE) == -1) {
        fprintf(stderr, "Could not read from data file: %s.\n", strerror(errno));
        exit(1);
    }
}

void DiskIO::closeFiles() {
    for(auto kv : filedescriptors){
        close(kv.second);
    }
}

/**
 * returns -1 if it does not have an open file
 */
int DiskIO::getOpenFileDescriptor(uint16_t segID) {
    auto got = filedescriptors.find(segID);
    if (got == filedescriptors.end()) {
        return -1;
    }
    else return got->second;
}


int DiskIO::getFileDescriptor(BufferFrame *bufferFrame) {
    int fd = getOpenFileDescriptor(bufferFrame->getSegmentID());
    if(fd == -1) {
        std::string filePath = DATA_PATH_PREFIX + std::to_string(bufferFrame->getSegmentID());
        fd = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        if(fd < 0) {
            fprintf(stderr, "Could not open data file \"%s\": %s.\n", filePath.c_str(), strerror(errno));
            exit(1);
        } else {
            filedescriptors.insert(std::pair<uint16_t, int>(bufferFrame->getSegmentID(), fd));
        }
    }
    return fd;
}