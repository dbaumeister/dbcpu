//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_LOCKEDCOLLECTION_H
#define PROJECT_LOCKEDCOLLECTION_H

#include <stdint.h>
#include <pthread.h>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "../exceptions/exceptions.h"


/*
 * Locks the collection during insert and find
 *
 */
template <class KEY_T, class ITEM_T>
class LockedCollection {
public:

    LockedCollection(){
        pthread_mutex_init(&content_mutex, NULL);
    }

    ~LockedCollection(){
        pthread_mutex_destroy(&content_mutex);
    }

    /* Inserts an item with a key value.
     * Locks the collection during insertion process.
     */
    void insert(KEY_T key, ITEM_T item){
        pthread_mutex_lock(&content_mutex);
        content.insert(std::pair<KEY_T, ITEM_T>(key, item));
        pthread_mutex_unlock(&content_mutex);
    }

    /*
     * Locks the collection during search process.
     * Returns the corresponding item, if key is found.
     * Throws and int exception NOT_FOUND otherwise
     *
     * use:
     * try {
     * ...
     * }
     * catch (int ex) {
     *      if(ex == NOT_FOUND) ...;
     * }
     */
    ITEM_T find(KEY_T key){
        pthread_mutex_lock(&content_mutex);
        auto got = content.find(key);
        pthread_mutex_unlock(&content_mutex);
        if (got == content.end()) {
            throw ITEM_NOT_FOUND_ERROR;
        }
        else return got->second;
    }

    /*
    * Locks the collection during remove process.
    * Removes the corresponding pair, if the collection contains the key.
    * Throws and int exception NOT_FOUND otherwise
    *
    * use:
    * try {
    * ...
    * }
    * catch (int ex) {
    *      if(ex == NOT_FOUND) ...;
    * }
    */
    bool remove(KEY_T key){
        pthread_mutex_lock(&content_mutex);
        unsigned int got = content.erase(key);
        pthread_mutex_unlock(&content_mutex);
        return (got == 0);
    }

    /*
     * Clears collection
     */
    void clear(){
        pthread_mutex_lock(&content_mutex);
        content.clear();
        pthread_mutex_unlock(&content_mutex);
    }

    /*
     * returns all items in a vector
     * this method locks the table for a long time, only use it carefully
     */
    std::vector<ITEM_T> getAll(){
        std::vector<ITEM_T> items;
        pthread_mutex_lock(&content_mutex);
        for(auto kv : content){
            items.push_back(kv.second);
        }
        pthread_mutex_unlock(&content_mutex);
        return items;
    }

private:
    std::unordered_map<KEY_T , ITEM_T> content;
    pthread_mutex_t content_mutex;

};

#endif //PROJECT_LOCKEDCOLLECTION_H
