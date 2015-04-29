//
// Created by dbaumeister on 21.04.15.
//


#include <string>

#include "BufferManager.h"


BufferFrame* BufferManager::fixPage(uint64_t id, bool isExclusive) {

    try{
        //Try to find the bufferFrame in our collection
        BufferFrame* bufferFrame =  collection.find(id);

        //lock until the other competitor unfixes its frame and we can use it
        //TODO deadlocks?
        bufferFrame->lockFrame(isExclusive);
        bufferFrame->setFixed(true);

        replacementStrategy.onUse(bufferFrame); //maybe call before lock..

        return bufferFrame;

    } catch (int exception) {
        if(exception == ITEM_NOT_FOUND_ERROR) {
            //we did not find the BufferFrame
            if(pageCount < pageCountMax){
                //if we have not reached our maxPageCount -> create a BufferFrame
                BufferFrame* bufferFrame = createBufferFrame(id);

                //lock until the other competitor unfixes its frame and we can use it
                //TODO deadlocks?
                bufferFrame->lockFrame(isExclusive);
                bufferFrame->setFixed(true);

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
                bufferFrame = replacementStrategy.popRemovable();
                if(bufferFrame == nullptr) {
                    printf("Cannot remove a frame... TODO: Need implementation for lock.");
                    exit(1);
                }

                bufferFrame->unlockFrame();

                //Remove the removable also from collection
                collection.remove(bufferFrame->getID());

                //write the removable BufferFrame to disk DONT free its RAM, we will need it
                // - important: only write to disk, when strategy makes space or BufferManagers Destructor is called
                if(bufferFrame->isDirty()){
                    //only  write back to disk, if any changes were done to the BufferFrame
                    io.writeToDisk(bufferFrame);
                }

                bufferFrame = recreateBufferFrame(id, bufferFrame); //reuse bufferFrameWrappers allocated memory
                //lock until the other competitor unfixes its frame and we can use it
                //TODO deadlocks?
                bufferFrame->lockFrame(isExclusive);
                bufferFrame->setFixed(true);

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
    bufferFrame->setFixed(false);
    bufferFrame->unlockFrame();
    replacementStrategy.onUse(bufferFrame);
}

BufferFrame* BufferManager::createBufferFrame(uint64_t id) {
    void* data = malloc(PAGESIZE);
    BufferFrame* bufferFrame = new BufferFrame(id, data);
    io.readFromDisk(bufferFrame);
    return bufferFrame;
}

BufferFrame* BufferManager::recreateBufferFrame(uint64_t id, BufferFrame* bufferFrame) {
    //Re-use bufferFrames allocated memory
    bufferFrame->clearControlDataAndSetID(id);
    io.readFromDisk(bufferFrame);
    return bufferFrame;
}

BufferManager::~BufferManager() {
    std::vector<BufferFrame*> bufferFrames = collection.getAll();
    for(BufferFrame* bufferFrame : bufferFrames) {
        if(bufferFrame->isDirty()) {
            io.writeToDisk(bufferFrame); //write all dirty frames to disk
        }
        delete(bufferFrame);
    }
    io.closeFiles();
    collection.clear();
}
