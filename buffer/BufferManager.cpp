//
// Created by dbaumeister on 21.04.15.
//


#include <string>
#include <sys/fcntl.h>
#include <unistd.h>

#include "BufferManager.h"


BufferFrame BufferManager::fixPage(uint64_t id, bool exclusive) {
    try{
        // If we already have the BufferFrame -> we are golden!
        BufferFrameWrapper* bufferFrameWrapper =  collection.find(id);
        // update our replacement strategy

        //throw an error, if an exclusive frame is requested, but already in use
        //TODO: instead of error: lock until the other competitor unfixes its frame and we can use it
        if(exclusive) {
            throw FRAME_ALREADY_IN_USE_ERROR;
        }

        replacementStrategy.onUse(bufferFrameWrapper);
        return bufferFrameWrapper->getBufferFrame();

    } catch (int exception) {
        if(exception == ITEM_NOT_FOUND_ERROR) {
            //we did not find the BufferFrame
            if(pageCount < pageCountMax){
                //if we have not reached our maxPageCount -> create a BufferFrame
                BufferFrameWrapper* bufferFrameWrapper = createBufferFrame(id);
                bufferFrameWrapper->setExclusive(exclusive);

                collection.insert(id, bufferFrameWrapper);
                ++pageCount;
                // update replacement strategy
                replacementStrategy.onCreate(bufferFrameWrapper);
                return bufferFrameWrapper->getBufferFrame();
            }
            else  {
                //if we have reached our maxPageCount

                //ask replacement strategy to make space (e.g. which BufferFrameWrapper can be removed)
                BufferFrameWrapper* bufferFrameWrapper;

                //In case there is nothing that can be removed -> throw error that insertion is currently not possible
                //TODO: instead of error: lock until something can be removed, and we can use the space
                try {
                    bufferFrameWrapper = replacementStrategy.popRemovable();
                } catch (int exc) {
                    if(exc == NO_REMOVABLE_FRAME_ERROR){
                        throw NO_REMOVABLE_FRAME_ERROR;
                    }
                }

                //Remove the removable also from collection
                collection.remove(bufferFrameWrapper->getID());

                //write the removable BufferFrame to disk DONT free its RAM, we will need it
                // - important: only write to disk, when strategy makes space or BufferManagers Destructor is called
                if(bufferFrameWrapper->isDirty()){
                    //only  write back to disk, if any changes were done to the BufferFrame
                    writeToDisk(bufferFrameWrapper);
                }

                bufferFrameWrapper = recreateBufferFrame(id, bufferFrameWrapper); //reuse bufferFrameWrappers allocated memory
                collection.insert(id, bufferFrameWrapper);

                //update replacement strategy
                replacementStrategy.onCreate(bufferFrameWrapper);

                return bufferFrameWrapper->getBufferFrame();
            }
        } else throw exception; //if there was another exception
    }
}


void BufferManager::unfixPage(BufferFrame &frame, bool isDirty) {
    try{
        BufferFrameWrapper* bufferFrameWrapper =  collection.find(frame.getID());
        if(isDirty){
            bufferFrameWrapper->setDirty(true); //only change dirty state if the caller set it to dirty
            // we don't want to magically clean our pages
        }
        bufferFrameWrapper->setExclusive(false);
        replacementStrategy.onUse(bufferFrameWrapper);

    } catch (int exception) {
        if(exception == ITEM_NOT_FOUND_ERROR) {
            printf("DEBUG: Could not unfixPage because the frame with ID %lu could not be found.\n"
             " Maybe it was unfixed before and has already been removed by the replacement strategy.", frame.getID());
        }
    }
}

BufferFrameWrapper* BufferManager::createBufferFrame(uint64_t id) {
    void* data = malloc(PAGESIZE);
    BufferFrameWrapper* bufferFrameWrapper = new BufferFrameWrapper(id, data);
    readFromDisk(bufferFrameWrapper);
    return bufferFrameWrapper;
}

BufferFrameWrapper* BufferManager::recreateBufferFrame(uint64_t id, BufferFrameWrapper* bufferFrameWrapper) {
    //does the same as createBufferFrame but reuses its allocated memory
    void* data = bufferFrameWrapper->getBufferFrame().getData();
    delete(bufferFrameWrapper);
    bufferFrameWrapper = new BufferFrameWrapper(id, data);
    readFromDisk(bufferFrameWrapper);
    return bufferFrameWrapper;
}

BufferManager::~BufferManager() {
    std::vector<BufferFrameWrapper*> bufferFrameWrappers = collection.getAll();
    for(BufferFrameWrapper* bufferFrameWrapper : bufferFrameWrappers) {
        if(bufferFrameWrapper->isDirty()) {
            writeToDisk(bufferFrameWrapper); //write all dirty frames to disk
        }
        free(bufferFrameWrapper->getBufferFrame().getData()); //free all data to avoid memory leaks
    }
    collection.clear();
}

void BufferManager::writeToDisk(BufferFrameWrapper* bufferFrameWrapper) {
    //persists the frame data, should not be interrupted!
    std::string filePath = DATA_PATH_PREFIX + std::to_string(bufferFrameWrapper->getSegmentID());

    printf("DEBUG: Trying to write to \"%s\" (pageID: %lu) ...\n", filePath.c_str(), bufferFrameWrapper->getPageID());

    int fd = open(filePath.c_str(), O_WRONLY | O_CREAT);
    if(fd < 0) {
        printf("ERROR: Could not open data file \"%s\" for writing.", filePath.c_str());
        throw IO_ERROR;
    }
    if(pwrite(fd, bufferFrameWrapper->getBufferFrame().getData(), PAGESIZE, bufferFrameWrapper->getPageID() * PAGESIZE) < 0) {
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

void BufferManager::readFromDisk(BufferFrameWrapper* bufferFrameWrapper) {
    //fill frames data
    //Get BufferedFrames by reading from disk at id, create it when it does not exist
    std::string filePath = DATA_PATH_PREFIX + std::to_string(bufferFrameWrapper->getSegmentID());

    printf("DEBUG: Trying to read from \"%s\" (pageID: %lu) ...\n", filePath.c_str(), bufferFrameWrapper->getPageID());

    int fd = open(filePath.c_str(), O_RDONLY | O_CREAT);
    if(fd < 0) {
        printf("ERROR: Could not open data file \"%s\" for reading.", filePath.c_str());
        throw IO_ERROR;
    }
    if(pread(fd, bufferFrameWrapper->getBufferFrame().getData(), PAGESIZE, bufferFrameWrapper->getPageID() * PAGESIZE) < 0) {
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
