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
    BufferFrame(uint64_t id, void* data) : id(id), data(data), dirty(false), user_count(0) {
        pthread_rwlock_init(&frame_rwlock, NULL);
    }

    ~BufferFrame(){
        for(int i = 0; i < user_count; ++i){
            unlockFrame();
        }
        pthread_rwlock_destroy(&frame_rwlock);
        free(data);
    }

    void* getData();

    uint64_t getID();
    uint16_t getSegmentID();
    uint64_t getPageID();

    bool isDirty();
    void setDirty(bool isDirty);

    void fix();
    void unfix();
    unsigned int getUserCount();

    void clearControlDataAndSetID(uint64_t id);

    void lockFrame(bool isExclusive);
    void unlockFrame();

private:
    void* data; //Data of PAGESIZE bytes
    uint64_t id;

    bool dirty;
    unsigned int user_count;

    //Latch
    pthread_rwlock_t frame_rwlock;


    void lockRead();
    void lockWrite();
};

#endif //PROJECT_BUFFERFRAME_H
