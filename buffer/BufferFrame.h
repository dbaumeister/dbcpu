//
// Created by dbaumeister on 21.04.15.
//

#ifndef PROJECT_BUFFERFRAME_H
#define PROJECT_BUFFERFRAME_H

#include <stdint.h>


#define PAGESIZE 4096 //Bytes

class BufferFrame{
public:
    BufferFrame(uint64_t id, void* data) : id(id), data(data) {}

    void* getData();

    uint64_t getID();

private:
    void* data; //Data of PAGESIZE bytes
    uint64_t id;
};

#endif //PROJECT_BUFFERFRAME_H
