//
// Created by dbaumeister on 22.04.15.
//

#ifndef PROJECT_LOCKEDCOLLECTION_H
#define PROJECT_LOCKEDCOLLECTION_H

#include <stdint.h>
#include <mutex>
#include <unordered_map>

#define NOT_FOUND -1

/*
 * Locks the collection during insert and find
 *
 */
template <class KEY_T, class ITEM_T>
class LockedCollection {
public:
    /* Inserts an item with a key value.
     * Locks the collection during insertion process.
     */
    void insert(KEY_T key, ITEM_T item){
        std::lock_guard<std::mutex> lock(content_mutex);
        content.insert(std::pair<KEY_T, ITEM_T>(key, item));
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
        std::lock_guard<std::mutex> lock(content_mutex);
        auto got = content.find(key);
        if (got == content.end()) {
            throw NOT_FOUND;
        }
        else return got->second;
    }

    uint64_t size(){
        return content.size();
    }
private:
    std::unordered_map<KEY_T , ITEM_T> content;
    std::mutex content_mutex;

};

#endif //PROJECT_LOCKEDCOLLECTION_H
