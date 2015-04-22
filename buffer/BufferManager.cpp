//
// Created by dbaumeister on 21.04.15.
//


#include <string>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "BufferManager.h"


BufferFrame BufferManager::fixPage(uint64_t id, bool exclusive) {
    //TODO lock if already in use exclusively etc.
    try{
        // If we already have the BufferFrame -> we are golden!
        BufferFrameWrapper bufferFrameWrapper =  collection.find(id);
        // update our replacement strategy
        replacementStrategy.update(bufferFrameWrapper);
        return bufferFrameWrapper.getBufferFrame();

    } catch (int exception) {
        if(exception == NOT_FOUND) {
            //we did not find the BufferFrame

            if(pageCount < maxPageCount){
                //if we have not reached our maxPageCount -> create a BufferFrame
                BufferFrameWrapper bufferFrameWrapper = createBufferFrame(id);
                collection.insert(id, bufferFrameWrapper);
                ++pageCount;
                // update replacement strategy
                replacementStrategy.create(bufferFrameWrapper);
                return bufferFrameWrapper.getBufferFrame();
            }
            else  {
                //if we have reached our maxPageCount -> ask replacement strategy
                //ask replacement strategy to make space (e.g. which BufferFrameWrapper can be removed) -> popRemovable
                BufferFrameWrapper bufferFrameWrapper = replacementStrategy.pop();

                //TODO in case there is nothing that can be removed -> throw error that insertion is currently not possible

                //Remove the removable also from collection
                collection.remove(bufferFrameWrapper.getID());

                //write the removable BufferFrame to disk DONT free its RAM, we will need it
                // - important: only write to disk, when strategy makes space or BufferManagers Destructor is called
                writeToDisk(bufferFrameWrapper);

                bufferFrameWrapper = recreateBufferFrame(id, bufferFrameWrapper); //reuse bufferFrameWrappers allocated memory
                collection.insert(id, bufferFrameWrapper);

                //update replacement strategy
                replacementStrategy.create(bufferFrameWrapper);

                return bufferFrameWrapper.getBufferFrame();
            }
        }
    }
}

BufferFrameWrapper BufferManager::createBufferFrame(uint64_t id) {
    void* data = malloc(PAGESIZE);
    BufferFrameWrapper bufferFrameWrapper = BufferFrameWrapper(STATE_NEW, id, data);
    readFromDisk(bufferFrameWrapper);
    return bufferFrameWrapper;
}

BufferFrameWrapper BufferManager::recreateBufferFrame(uint64_t id, BufferFrameWrapper &bufferFrameWrapper) {
    //Get BufferedFrames by reading from disk at id, create it when it does not exist
    //does the same as createBufferFrame but reuses its allocated memory
    void* data = bufferFrameWrapper.getBufferFrame().getData();
    bufferFrameWrapper.setBufferFrame(BufferFrame(id, data));
    readFromDisk(bufferFrameWrapper);
    return  bufferFrameWrapper;
}

void BufferManager::writeToDisk(BufferFrameWrapper &frame) {
    //persists the frame data, should not be interrupted!
    int fd = open(std::to_string(frame.getSegmentID()).c_str(), O_RDONLY | O_CREAT);
    if(fd < 0) {
        printf("Could not open data file.");
        throw IO_ERROR;
    }
    if(pwrite(fd, frame.getBufferFrame().getData(), PAGESIZE, frame.getPageID() * PAGESIZE) < 0) {
        printf("Could not write in data file.");
        throw IO_ERROR;
    }
    if(close(fd) < 0) {
        printf("Could not close data file.");
        throw IO_ERROR;
    };
}

void BufferManager::readFromDisk(BufferFrameWrapper &frame) {
    //fill frames data
    int fd = open(std::to_string(frame.getSegmentID()).c_str(), O_RDONLY | O_CREAT);
    if(fd < 0) {
        printf("Could not open data file.");
        throw IO_ERROR;
    }
    if(pread(fd, frame.getBufferFrame().getData(), PAGESIZE, frame.getPageID() * PAGESIZE) < 0) {
        printf("Could not read from data file.");
        throw IO_ERROR;
    }
    if(close(fd) < 0) {
        printf("Could not close data file.");
        throw IO_ERROR;
    };
}
