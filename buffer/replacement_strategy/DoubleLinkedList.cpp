//
// Created by dbaumeister on 29.04.15.
//

#include "DoubleLinkedList.h"

bool DoubleLinkedList::isEmpty() {
    return first == nullptr;
}

void DoubleLinkedList::push_back(BufferFrame *bufferFrame) {
    Element* e;

    if(table.empty()){
        e = new Element(nullptr, nullptr, bufferFrame);
        first = e;
    }
    else {
        e = new Element(nullptr, last, bufferFrame);
        last->next = e;
    }

    last = e;

    table.insert(std::pair<BufferFrame*, Element*>(bufferFrame, e));
}

BufferFrame *DoubleLinkedList::pop_unfixed() {
    //assume, that we  are not empty, otherwise the layer above would have made a mistake

    if(first == nullptr) return nullptr;

    Element* e = first;

    while(e != nullptr && e->bufferFrame->getUserCount() > 0){
        e = e->next;
    }

    if(e == nullptr) return nullptr;

    BufferFrame* bufferFrame = e->bufferFrame;

    if(e == first && e == last) {
        first = nullptr;
        last = nullptr;
    } else if(e == first){
        first = first->next;
        first->prev = nullptr;
    } else if(e == last){
        last = last->prev;
        last->next = nullptr;
    } else {
        e->prev->next = e->next;
        e->next->prev = e->prev;
    }
    delete(e);

    table.erase(bufferFrame);

    return bufferFrame;
}

void DoubleLinkedList::remove(BufferFrame *bufferFrame) {
    //assume, that bufferFrame is in list already
    auto got = table.find(bufferFrame);
    if(got == table.end()) {
        return;
    }
    else {
        Element* e = got->second;
        //Case 1: e is first and last
        if(e == first && e == last) {
            first = nullptr;
            last = nullptr;
        } else if(e == first){
            first = first->next;
            first->prev = nullptr;
        } else if(e == last){
            last = last->prev;
            last->next = nullptr;
        } else {
            e->prev->next = e->next;
            e->next->prev = e->prev;
        }
        delete(e);
    }
    table.erase(bufferFrame);
}

bool DoubleLinkedList::contains(BufferFrame *bufferFrame) {
    auto got = table.find(bufferFrame);
    return got != table.end();
}

/**
 * Pops the first clean element, nullptr otherwise
 */
BufferFrame *DoubleLinkedList::pop_clean() {

    if(first == nullptr) return nullptr;

    Element* e = first;

    //skip as long as e is not null and contains a dirty BufferFrame or one that has users
    while(e != nullptr && (e->bufferFrame->isDirty() || e->bufferFrame->getUserCount() > 0)){
        e = e->next;
    }
    //-> e is either nullptr or contains a clean unused BufferFrame
    if(e == nullptr) return nullptr;

    BufferFrame* bufferFrame = e->bufferFrame;

    if(e == first && e == last) {
        first = nullptr;
        last = nullptr;
    } else if(e == first){
        first = first->next;
        first->prev = nullptr;
    } else if(e == last){
        last = last->prev;
        last->next = nullptr;
    } else {
        e->prev->next = e->next;
        e->next->prev = e->prev;
    }
    delete(e);

    table.erase(bufferFrame);

    return bufferFrame;
}
