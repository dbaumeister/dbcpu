//
// Created by dbaumeister on 19.05.15.
//

#ifndef PROJECT_BTREE_H
#define PROJECT_BTREE_H

#include <vector>
#include <atomic>
#include <pthread.h>
#include "../slottedpages/segment/TID.h"


template <class KeyT, class CompT>
class BTreeNode{
public:

    BTreeNode() {
        numUsers = 0;
        pthread_rwlock_init(&nodeLock, NULL);
    }

    ~BTreeNode() {
        while(numUsers > 0){
            unlock();
        }
        pthread_rwlock_destroy(&nodeLock);
    }

    void setWriteLock(){
        pthread_rwlock_wrlock(&nodeLock);
        ++numUsers;
    }

    void setReadLock(){
        pthread_rwlock_rdlock(&nodeLock);
        ++numUsers;
    }

    void unlock(){
        --numUsers;
        pthread_rwlock_unlock(&nodeLock);
    }

    bool isLeaf;
    uint64_t n;
    BTreeNode* next;

    std::vector<std::pair<KeyT, BTreeNode&>> children;
    std::vector<std::pair<KeyT, TID>> entries;

private:
    std::atomic<unsigned int> numUsers;
    pthread_rwlock_t nodeLock;
};

template <class KeyT, class CompT>
class BTree{
public:

    BTree(uint64_t n) {
        root = BTreeNode();
        root.n = n;
        root.isLeaf = false;
    }

    void insert(KeyT  key, TID tid);
    void erase(KeyT key);
    TID lookup(KeyT key);

private:
    BTreeNode root;

};

#endif //PROJECT_BTREE_H
