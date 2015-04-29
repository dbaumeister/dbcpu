//
// Created by dbaumeister on 21.04.15.
//

#ifndef PROJECT_BUFFERFRAME_H
#define PROJECT_BUFFERFRAME_H

#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>


#define PAGESIZE 4096 //Bytes

class BufferFrame{
public:
    BufferFrame(uint64_t id, void* data) : id(id), data(data), exclusive(false), dirty(false), state(0) {
        pthread_mutex_init(&frame_mutex, NULL);
    }

    ~BufferFrame(){
        pthread_mutex_destroy(&frame_mutex);
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

private:
    void* data; //Data of PAGESIZE bytes
    uint64_t id;

    //TODO: Latch (mutex)
    uint16_t state; //State for replacement Strategy
    bool dirty, exclusive;

    pthread_mutex_t frame_mutex;
};

#endif //PROJECT_BUFFERFRAME_H
