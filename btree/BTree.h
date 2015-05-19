//
// Created by dbaumeister on 19.05.15.
//

#ifndef PROJECT_BTREE_H
#define PROJECT_BTREE_H

#include <vector>
#include <atomic>
#include <pthread.h>
#include "../slottedpages/segment/TID.h"
#include "../slottedpages/segment/SPSegment.h"

template <class KeyT, class CompT>
class BTreeNode{
public:

    BTreeNode(SPSegment& segment) : segment(segment){
        numUsers = 0;
        pthread_rwlock_init(&nodeLock, NULL);
    }

    ~BTreeNode() {
        while(numUsers > 0){
            unlock();
        }
        pthread_rwlock_destroy(&nodeLock);
    }

    void unlock() {
        --numUsers;
        pthread_rwlock_unlock(&nodeLock);
    }

    void setReadLock() {
        pthread_rwlock_rdlock(&nodeLock);
        ++numUsers;
    }

    void setWriteLock() {
        pthread_rwlock_wrlock(&nodeLock);
        ++numUsers;
    }

    bool isLeaf;
    BTreeNode* next;

    std::vector<std::pair<KeyT, BTreeNode&>> children;
    std::vector<std::pair<KeyT, TID>> entries;

private:
    std::atomic<unsigned int> numUsers;
    pthread_rwlock_t nodeLock;
    SPSegment segment;
};

template <class KeyT, class CompT>
class BTree{
public:

    BTree(SPSegment& segment) : segment(segment), root(new BTreeNode<KeyT, CompT>(segment)) {
        root->isLeaf = false;
        _size = 0;
    }

    void insert(KeyT key, TID& tid) {

    }

    bool erase(KeyT key) {
        return false;
    }

    bool lookup(KeyT key, TID& tid) {
        return false;
    }


    uint64_t size(){
        return _size;
    }

private:
    uint64_t _size;
    BTreeNode<KeyT, CompT>* root;
    SPSegment segment;
};

#endif //PROJECT_BTREE_H
