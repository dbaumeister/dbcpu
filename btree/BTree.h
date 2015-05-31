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

const uint64_t NO_NEIGHBOR = ~(uint64_t) 0;


template<class KeyT, class CompT>
class BTree {
    class BTreeNode {
    public:
        uint16_t numEntries;
        bool isLeaf = true;
    };

    class BTreeInner : public BTreeNode{
    public:
        struct KeyPage {
            KeyT key;
            uint64_t pageID;
        };

        KeyPage children[(PAGESIZE - sizeof(BTreeNode)) / sizeof(KeyPage)];

        uint64_t getLastPageID() {
            return children[BTreeNode::numEntries - 1].pageID;
        }

        bool isFull() {
            return (PAGESIZE - sizeof(BTreeNode)) / sizeof(KeyPage) - 1 <= BTreeNode::numEntries;
        }

        void insert(KeyT key, uint64_t &pageID) {
            CompT comp;
            uint16_t i = 0;
            for(; i < BTreeNode::numEntries; ++i){
                if(comp(key, children[i].key)) break;
            }

            memcpy(&children[i+1], &children[i], (BTreeNode::numEntries - i) * sizeof(KeyPage));
            KeyPage keyPage;
            keyPage.key = key;
            keyPage.pageID = pageID;
            children[i] = keyPage;

            ++BTreeNode::numEntries;
        }

        /*
         * This method should only be called after a split -> the parent node has to be updated
         */
        bool updatePageID(KeyT key, uint64_t newPageID){
            for(uint16_t i = 0; i < BTreeNode::numEntries; ++i){
                if(memcmp(&key, &(children[i].key), sizeof(KeyT))){
                    children[i].pageID = newPageID;
                    return true;
                }
            }
            return false;
        }

        void split(BTreeInner* right) {
            uint16_t numEntriesLeft = BTreeNode::numEntries / (uint16_t)2;
            right->BTreeNode::numEntries = BTreeNode::numEntries - numEntriesLeft;

            memcpy(&right->children[0], &children[numEntriesLeft], right->BTreeNode::numEntries * sizeof(KeyPage));
            BTreeNode::numEntries = numEntriesLeft;
        }

        uint64_t getChildID(KeyT key) {
            CompT c;
            for(uint16_t i = 0; i < BTreeNode::numEntries; ++i){
                if(c(key, children[i].key)){
                    return children[i].pageID;
                }
            }
            return 0;
        }

        KeyT getHighestKey(){
            return children[BTreeNode::numEntries-1].key;
        }
    };

    class BTreeLeaf : public BTreeNode{
    public:
        struct KeyTID {
            KeyT key;
            TID tid;
        };
        uint64_t rightNeighbor = NO_NEIGHBOR;
        KeyTID entries[(PAGESIZE - sizeof(BTreeNode) - sizeof(uint64_t)) / sizeof(KeyTID)];

        bool isFull() {
            return (PAGESIZE - sizeof(BTreeNode) - sizeof(uint64_t)) / sizeof(KeyTID) - 1 <= BTreeNode::numEntries;
        }

        void insert(KeyT key, TID &tid) {
            CompT comp;
            uint16_t i = 0;
            for(; i < BTreeNode::numEntries; ++i){
                if(comp(key, entries[i].key)) break;
            }

            memcpy(&entries[i+1], &entries[i], (BTreeNode::numEntries - i) * sizeof(KeyTID));
            KeyTID keyTID;
            keyTID.key = key;
            keyTID.tid = tid;
            entries[i] = keyTID;

            ++BTreeNode::numEntries;
        }

        void split(BTreeLeaf* right, uint64_t rightID) {
            right->rightNeighbor = rightNeighbor; //Bplus-Tree
            rightNeighbor = rightID;

            uint16_t numEntriesLeft = BTreeNode::numEntries / (uint16_t)2;
            right->BTreeNode::numEntries = BTreeNode::numEntries - numEntriesLeft;

            memcpy(&right->entries[0], &entries[numEntriesLeft], right->BTreeNode::numEntries * sizeof(KeyTID));
            BTreeNode::numEntries = numEntriesLeft;
        }

        bool lookup(KeyT key, TID& tid){
            for(uint16_t i = 0; i < BTreeNode::numEntries; ++i){
                if(memcmp(&key, &(entries[i].key), sizeof(KeyT))){
                    tid = entries[i].tid;
                    return true;
                }
            }
            return false;
        }

        KeyT getHighestKey(){
            return entries[BTreeNode::numEntries - 1].key;
        }
    };


public:
    BTree(BufferManager &bm, uint64_t segmentID) : bufferManager(bm), segmentID(segmentID << 48), rootPageID(0),
                                                   numPages(1) {
        _size = 0;
        BufferFrame *bufferFrame = bufferManager.fixPage(createID(rootPageID), true);
        BTreeNode* node = (BTreeNode*) bufferFrame->getData();
        node->isLeaf = true;
        node->numEntries = 0;
        bufferManager.unfixPage(bufferFrame, true);
    }

    void insert(KeyT key, TID &tid);

    bool erase(KeyT key);

    bool lookup(KeyT key, TID &tid);

    uint64_t size() {
        return _size;
    }

private:
    std::atomic<uint64_t> numPages;
    std::atomic<uint64_t> _size;
    uint64_t rootPageID;

    BufferManager &bufferManager;
    uint64_t segmentID;

    /*
     * returns an ID that is used to access frames within the bufferManager
     */
    uint64_t createID(uint64_t pageID) {
        return segmentID + pageID;
    }

    //TODO vererbung lösen lassen
    bool isFull(BTreeNode *node) {
        if (node->isLeaf) return reinterpret_cast<BTreeLeaf*>(node)->isFull();
        else return reinterpret_cast<BTreeInner*>(node)->isFull();
    }


    //TODO vererbung lösen lassen
    void split(BTreeNode *node, BTreeNode* right, uint64_t rightID) {
        right->isLeaf = node->isLeaf;
        if (node->isLeaf) reinterpret_cast<BTreeLeaf *>(node)->split(reinterpret_cast<BTreeLeaf*>(right), rightID);
        else reinterpret_cast<BTreeInner *> (node)->split(reinterpret_cast<BTreeInner *>(right));
    }


    //TODO vererbung lösen lassen
    KeyT getHighestKey(BTreeNode* node){
        if(node->isLeaf) return reinterpret_cast<BTreeLeaf*>(node)->getHighestKey();
        else return reinterpret_cast<BTreeInner*>(node)->getHighestKey();
    }

};

template<class KeyT, class CompT>
void BTree<KeyT, CompT>::insert(KeyT key, TID &tid) {

    BufferFrame *bufferFrameCurrent = bufferManager.fixPage(createID(rootPageID), true);
    BTreeNode *current = (BTreeNode *) bufferFrameCurrent->getData();

    if (isFull(current)) {
        uint64_t rightPageID = numPages;
        ++numPages;
        BufferFrame* bufferFrameRight = bufferManager.fixPage(createID(rightPageID), true);
        BTreeNode *right = (BTreeNode*) bufferFrameRight->getData();

        split(current, right, rightPageID);

        // 1. fix new page
        BufferFrame *bufferFrameNewRoot = bufferManager.fixPage(createID(numPages), true);
        BTreeInner *newRoot = (BTreeInner *) bufferFrameNewRoot->getData();
        newRoot->isLeaf = false;

        // 2. insert rootPageID (Key is the most right of current) and rightPageID (Key is the most right of right) into new page
        KeyT leftKey = getHighestKey(current);
        KeyT rightKey = getHighestKey(right);
        newRoot->insert(leftKey, rootPageID);
        newRoot->insert(rightKey, rightPageID);

        // 3. set new page as root
        rootPageID = numPages;
        numPages++;

        std::swap(bufferFrameNewRoot, bufferFrameCurrent);
        current = (BTreeNode*) newRoot;

        bufferManager.unfixPage(bufferFrameNewRoot, true);
        bufferManager.unfixPage(bufferFrameRight, true);
    }


    for (; ;) {
        if (current->isLeaf) break;

        BTreeInner *currentInner = reinterpret_cast<BTreeInner *>(current);
        uint64_t childPageID = currentInner->getChildID(key);

        BufferFrame *bufferFrameChild = bufferManager.fixPage(createID(childPageID), true);
        BTreeNode *child = (BTreeNode *) bufferFrameChild->getData();

        bool hasBeenSplit = false;
        if (isFull(child)) {
            uint64_t rightPageID = numPages;
            ++numPages;
            BufferFrame* bufferFrameRight = bufferManager.fixPage(createID(rightPageID), true);
            BTreeNode *right = (BTreeNode*) bufferFrameRight->getData();


            split(child, right, rightPageID);

            // insert split key/pageid in current
            KeyT leftKey = getHighestKey(child);
            KeyT rightKey = getHighestKey(right);
            currentInner->insert(leftKey, childPageID);
            currentInner->updatePageID(rightKey, rightPageID); //right key already exists in parent node -> just update it
            hasBeenSplit = true;

            bufferManager.unfixPage(bufferFrameRight, true);
        }

        // swap current and child
        std::swap(bufferFrameChild, bufferFrameCurrent);
        std::swap(child, current);

        // unfix child bufferframe (the old current)
        bufferManager.unfixPage(bufferFrameChild, hasBeenSplit);

    }

    //We always enter a non-full leaf
    BTreeLeaf *currentLeaf = reinterpret_cast<BTreeLeaf *>(current);
    currentLeaf->insert(key, tid);
    ++_size;
    bufferManager.unfixPage(bufferFrameCurrent, true);
}

template<class KeyT, class CompT>
bool BTree<KeyT, CompT>::erase(KeyT key) {
    return false;
}

template<class KeyT, class CompT>
bool BTree<KeyT, CompT>::lookup(KeyT key, TID &tid) {
    BufferFrame *bufferFrameCurrent = bufferManager.fixPage(createID(rootPageID), false);
    BTreeNode *current = (BTreeNode *) bufferFrameCurrent->getData();

    for(;;){
        if (current->isLeaf) break;

        BTreeInner *currentInner = reinterpret_cast<BTreeInner *>(current);
        uint64_t childPageID = currentInner->getChildID(key);

        BufferFrame *bufferFrameChild = bufferManager.fixPage(createID(childPageID), true);
        BTreeNode *child = (BTreeNode *) bufferFrameChild->getData();

        // swap current and child
        std::swap(bufferFrameChild, bufferFrameCurrent);
        std::swap(child, current);

        // unfix child bufferframe (the old current)
        bufferManager.unfixPage(bufferFrameChild, false);

    }

    BTreeLeaf *leaf = reinterpret_cast<BTreeLeaf *>(current);
    if(leaf->lookup(key, tid)){
        bufferManager.unfixPage(bufferFrameCurrent, false);
        return true;
    }

    bufferManager.unfixPage(bufferFrameCurrent, false);
    return false;
}


#endif //PROJECT_BTREE_H
