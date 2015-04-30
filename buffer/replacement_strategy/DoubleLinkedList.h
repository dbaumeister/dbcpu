//
// Created by dbaumeister on 29.04.15.
//

#ifndef PROJECT_LRU_H
#define PROJECT_LRU_H

#include <unordered_map>
#include "../BufferFrame.h"

struct Element{
    Element* next;
    Element* prev;
    BufferFrame* bufferFrame;

    explicit Element(Element* nextElement, Element* previousElement, BufferFrame* bufferFrame)
            : next(nextElement), prev(previousElement), bufferFrame(bufferFrame){}

};

class DoubleLinkedList{
public:

    DoubleLinkedList() : first(nullptr), last(nullptr){}
    ~DoubleLinkedList(){
        for(auto kv : table){
            delete(kv.second);
        }
    }

    void push_back(BufferFrame* bufferFrame);
    BufferFrame* pop_unfixed();
    BufferFrame* pop_clean();

    void remove(BufferFrame* bufferFrame);
    bool contains(BufferFrame* bufferFrame);

private:
    std::unordered_map<BufferFrame*, Element*> table; //Search structure to find the element quickly

    Element* first; //contains an actual Element or nullptr if list is empty
    Element* last; //contains an actual Element or nullptr if list is empty
};

#endif //PROJECT_LRU_H
