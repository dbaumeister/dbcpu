//
// Created by dbaumeister on 21.04.15.
//


#include <string>

#include "BufferManager.h"

BufferFrame* BufferManager::fixPage(uint64_t id, bool isExclusive) {

    BufferFrame* bufferFrame;

    pthread_mutex_lock(&global_lock);
    auto it = frames.find(id);

    if(it != frames.end()){
        //success: we found it
        bufferFrame = it->second;

        bufferFrame->lockFrame(isExclusive);
        bufferFrame->fix();

        replacementStrategy.onUse(bufferFrame);

        pthread_mutex_unlock(&global_lock);
        return bufferFrame;
    }

    //If we did not find it:
    if(pageCount < pageCountMax){
        //if we have not reached our maxPageCount -> create a BufferFrame
        bufferFrame = createBufferFrame(id);

        bufferFrame->lockFrame(isExclusive); //lock until the other competitor unfixes its frame and we can use it
        bufferFrame->fix();

        frames.insert(std::pair<uint64_t, BufferFrame*>(id, bufferFrame));
        ++pageCount;

        replacementStrategy.onCreate(bufferFrame); // update replacement strategy

        pthread_mutex_unlock(&global_lock);
        return bufferFrame;
    }

    //If we did not find it and we have reached our maxPageCount:
    //ask replacement strategy to make space (e.g. which BufferFrameWrapper can be removed)

    //In case there is nothing that can be removed -> throw error that insertion is currently not possible
    bufferFrame = replacementStrategy.popRemovable();
    if(bufferFrame == nullptr) {
        fprintf(stderr, "Cannot remove a frame.\n");
        exit(1);
    }

    frames.erase(bufferFrame->getID()); //Remove the removable bufferFrame also from collection

    if(bufferFrame->isDirty()) { //only  write back to disk, if any changes were done to the BufferFrame
        io.writeToDisk(bufferFrame); //write the removable BufferFrame to disk
    }

    bufferFrame = recreateBufferFrame(id, bufferFrame); //reuse bufferFrames allocated memory

    bufferFrame->lockFrame(isExclusive); //lock until the other competitor unfixes its frame and we can use it
    bufferFrame->fix();

    frames.insert(std::pair<uint64_t, BufferFrame*>(id, bufferFrame));
    replacementStrategy.onCreate(bufferFrame); //update replacement strategy

    pthread_mutex_unlock(&global_lock);
    return bufferFrame;
}


void BufferManager::unfixPage(BufferFrame* bufferFrame, bool isDirty) {
    //As it is our code, we can rely that frame is ours -> we don't search it in the collection
    if(isDirty){
        bufferFrame->setDirty(true); //only change dirty state if the caller set it to dirty
        // we don't want to magically clean our pages
    }
    bufferFrame->unfix();
    bufferFrame->unlockFrame();
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
    pthread_mutex_lock(&global_lock);
    for(auto it : frames) {
        BufferFrame* bufferFrame = it.second;
        if(bufferFrame->isDirty()) {
            io.writeToDisk(bufferFrame); //write all dirty frames to disk
        }
        delete(bufferFrame);
    }
    io.closeFiles();
    frames.clear();
    pthread_mutex_unlock(&global_lock);
    pthread_mutex_destroy(&global_lock);
}