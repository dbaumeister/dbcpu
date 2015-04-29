//
// Created by dbaumeister on 21.04.15.
//

#ifndef PROJECT_BUFFERFRAME_H
#define PROJECT_BUFFERFRAME_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>


#define PAGESIZE 4096 //Bytes

class BufferFrame{
public:
    BufferFrame(uint64_t id, void* data) : id(id), data(data), exclusive(false), dirty(false), fixed(false) {
        pthread_rwlock_init(&frame_rwlock, NULL);
    }

    ~BufferFrame(){
        unlockFrame();
        pthread_rwlock_destroy(&frame_rwlock);
        free(data);
    }

    void* getData();

    uint64_t getID();
    uint16_t getSegmentID();
    uint64_t getPageID();

    bool isDirty();
    void setDirty(bool isDirty);

    bool isExclusive();
    void setExclusive(bool isExclusive);

    bool isFixed();
    void setFixed(bool isFixed);

    void clearControlDataAndSetID(uint64_t id);

    void lockFrame(bool isExclusive);
    void unlockFrame();

private:
    void* data; //Data of PAGESIZE bytes
    uint64_t id;

    bool dirty, exclusive, fixed;

    //Latch
    pthread_rwlock_t frame_rwlock;


    void lockRead();
    void lockWrite();
};

#endif //PROJECT_BUFFERFRAME_H
