//
// Created by dbaumeister on 21.04.15.
//


#include <string>
#include <sys/fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include "BufferManager.h"


BufferFrame* BufferManager::fixPage(uint64_t id, bool exclusive) {

    try{
        // If we already have the BufferFrame -> we are golden!
        BufferFrame* bufferFrame =  collection.find(id);
        // update our replacement strategy

        //throw an error, if an exclusive frame is requested, but already in use
        //TODO: instead of error: lock until the other competitor unfixes its frame and we can use it
        if(exclusive) {
            throw FRAME_ALREADY_IN_USE_ERROR;
        }

        replacementStrategy.onUse(bufferFrame);
        return bufferFrame;

    } catch (int exception) {
        if(exception == ITEM_NOT_FOUND_ERROR) {
            //we did not find the BufferFrame
            if(pageCount < pageCountMax){
                //if we have not reached our maxPageCount -> create a BufferFrame
                BufferFrame* bufferFrame = createBufferFrame(id);
                bufferFrame->setExclusive(exclusive);

                collection.insert(id, bufferFrame);
                ++pageCount;
                // update replacement strategy
                replacementStrategy.onCreate(bufferFrame);
                return bufferFrame;
            }
            else  {
                //if we have reached our maxPageCount
                //ask replacement strategy to make space (e.g. which BufferFrameWrapper can be removed)
                BufferFrame* bufferFrame;

                //In case there is nothing that can be removed -> throw error that insertion is currently not possible
                //TODO: instead of error: trylock until something can be removed, and we can use the space
                try {
                    bufferFrame = replacementStrategy.popRemovable();
                } catch (int exc) {
                    if(exc == NO_REMOVABLE_FRAME_ERROR){
                        throw NO_REMOVABLE_FRAME_ERROR;
                    }
                }

                //Remove the removable also from collection
                collection.remove(bufferFrame->getID());

                //write the removable BufferFrame to disk DONT free its RAM, we will need it
                // - important: only write to disk, when strategy makes space or BufferManagers Destructor is called
                if(bufferFrame->isDirty()){
                    //only  write back to disk, if any changes were done to the BufferFrame
                    writeToDisk(bufferFrame);
                }

                bufferFrame = recreateBufferFrame(id, bufferFrame); //reuse bufferFrameWrappers allocated memory
                bufferFrame->setExclusive(exclusive);

                collection.insert(id, bufferFrame);

                //update replacement strategy
                replacementStrategy.onCreate(bufferFrame);

                return bufferFrame;
            }
        } else throw exception; //if there was another exception
    }
}


void BufferManager::unfixPage(BufferFrame* bufferFrame, bool isDirty) {
    //As it is our code, we can rely that frame is ours -> we don't search it in the collection
    if(isDirty){
        bufferFrame->setDirty(true); //only change dirty state if the caller set it to dirty
        // we don't want to magically clean our pages
    }
    bufferFrame->setExclusive(false);
    replacementStrategy.onUse(bufferFrame);
}

BufferFrame* BufferManager::createBufferFrame(uint64_t id) {
    void* data = malloc(PAGESIZE);
    BufferFrame* bufferFrame = new BufferFrame(id, data);
    readFromDisk(bufferFrame);
    return bufferFrame;
}

BufferFrame* BufferManager::recreateBufferFrame(uint64_t id, BufferFrame* bufferFrame) {
    //Re-use bufferFrames allocated memory
    bufferFrame->clearControlDataAndSetID(id);
    readFromDisk(bufferFrame);
    return bufferFrame;
}

BufferManager::~BufferManager() {
    std::vector<BufferFrame*> bufferFrames = collection.getAll();
    for(BufferFrame* bufferFrame : bufferFrames) {
        if(bufferFrame->isDirty()) {
            writeToDisk(bufferFrame); //write all dirty frames to disk
        }
        delete(bufferFrame);
    }
    collection.clear();
}

void BufferManager::writeToDisk(BufferFrame* bufferFrame) {
    //persists the frame data, should not be interrupted!
    std::string filePath = DATA_PATH_PREFIX + std::to_string(bufferFrame->getSegmentID());

    printf("DEBUG: Trying to write to \"%s\" (pageID: %lu) ...\n", filePath.c_str(), bufferFrame->getPageID());

    int fd = open(filePath.c_str(), O_WRONLY | O_CREAT);
    if(fd < 0) {
        printf("ERROR: Could not open data file \"%s\" for writing.", filePath.c_str());
        throw IO_ERROR;
    }
    if(pwrite(fd, bufferFrame->getData(), PAGESIZE, bufferFrame->getPageID() * PAGESIZE) < 0) {
        printf("ERROR: Could not write in data file.");
        close(fd); //it does not matter, if we could close the file, as we are throwing an error anyway. at least we try to close it
        throw IO_ERROR;
    }
    if(close(fd) < 0) {
        printf("ERROR: Could not close data file.");
        throw IO_ERROR;
    };

    printf("DEBUG: Writing to \"%s\" was successful!\n", filePath.c_str());
}

void BufferManager::readFromDisk(BufferFrame* bufferFrame) {
    //fill frames data
    //Get BufferedFrames by reading from disk at id, create it when it does not exist
    std::string filePath = DATA_PATH_PREFIX + std::to_string(bufferFrame->getSegmentID());

    printf("DEBUG: Trying to read from \"%s\" (pageID: %lu) ...\n", filePath.c_str(), bufferFrame->getPageID());

    int fd = open(filePath.c_str(), O_RDONLY | O_CREAT);
    if(fd < 0) {
        printf("ERROR: Could not open data file \"%s\" for reading.", filePath.c_str());
        throw IO_ERROR;
    }
    if(pread(fd, bufferFrame->getData(), PAGESIZE, bufferFrame->getPageID() * PAGESIZE) < 0) {
        printf("ERROR: Could not read from data file.");
        close(fd); //it does not matter if we could close the file, as we are throwing an error anyway. at least we try to close it
        throw IO_ERROR;
    }
    if(close(fd) < 0) {
        printf("ERROR: Could not close data file.");
        throw IO_ERROR;
    };

    printf("DEBUG: Reading from \"%s\" was successful!\n", filePath.c_str());
}
