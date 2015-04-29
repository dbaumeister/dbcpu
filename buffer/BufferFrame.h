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
    BufferFrame(uint64_t id, void* data) : id(id), data(data), exclusive(false), dirty(false), state(0) {
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

    void clearControlDataAndSetID(uint64_t id);

    void setState(uint16_t state);

    void lockRead();
    void lockWrite();
    void unlockFrame();

private:
    void* data; //Data of PAGESIZE bytes
    uint64_t id;

    uint16_t state; //State for replacement Strategy
    bool dirty, exclusive;

    //Latch
    pthread_rwlock_t frame_rwlock;
};

#endif //PROJECT_BUFFERFRAME_H
