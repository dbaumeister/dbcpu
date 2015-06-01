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
        uint16_t numEntries = 0;
        bool isLeaf = true;
    };

    class BTreeInner : public BTreeNode {
    public:
        struct KeyPage {
            KeyT key;
            uint64_t pageID;
        };

        KeyPage children[(PAGESIZE - sizeof(BTreeNode)) / sizeof(KeyPage)];

        bool isFull() {
            return (PAGESIZE - sizeof(BTreeNode)) / sizeof(KeyPage) - 1 <= BTreeNode::numEntries;
        }

        void insert(KeyT key, uint64_t pageID) {
            CompT comp;
            uint16_t i = 0;
            for (; i < BTreeNode::numEntries; ++i) {
                if (comp(key, children[i].key)) break;
            }

            memmove(&children[i + 1], &children[i], (BTreeNode::numEntries - i) * sizeof(KeyPage));
            KeyPage keyPage;
            keyPage.key = key;
            keyPage.pageID = pageID;
            children[i] = keyPage;

            ++BTreeNode::numEntries;
        }

        void split(BTreeInner *right) {
            uint16_t numEntriesLeft = BTreeNode::numEntries / (uint16_t) 2;
            right->numEntries = BTreeNode::numEntries - numEntriesLeft;

            memcpy(&right->children[0], &children[numEntriesLeft], right->numEntries * sizeof(KeyPage));
            BTreeNode::numEntries = numEntriesLeft;
        }

        void updateKeyOfPageID(KeyT key, uint64_t pageID) {
            for (uint16_t i = 0; i < BTreeNode::numEntries; ++i) {
                if (memcmp(&pageID, &children[i].pageID, sizeof(uint64_t)) == 0) {
                    children[i].key = key;
                    return;
                }
            }
        }

        uint64_t getChildID(KeyT key) {
            CompT comp;
            uint16_t i;
            for (i = 0; i < BTreeNode::numEntries - 1; ++i) {
                if (!comp(children[i].key, key)) { // key not bigger than this entry -> key <= children[i].key
                    return children[i].pageID;
                }
            }
            return children[i].pageID; //return last pageID
        }

        KeyT getHighestKey() {
            return children[BTreeNode::numEntries - 1].key;
        }
    };

    class BTreeLeaf : public BTreeNode {
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

        bool erase(KeyT key) {
            for (uint16_t i = 0; i < BTreeNode::numEntries; ++i) {
                if (memcmp(&key, &entries[i].key, sizeof(KeyT)) == 0) {
                    memmove(&entries[i], &entries[i + 1], (BTreeNode::numEntries - i - 1) * sizeof(KeyTID));
                    --BTreeNode::numEntries;
                    return true;
                }
            }
            return false;
        }

        void insert(KeyT key, TID tid) {
            CompT comp;
            uint16_t i = 0;
            for (; i < BTreeNode::numEntries; ++i) {
                if (comp(key, entries[i].key)) break;
            }
            memmove(&entries[i + 1], &entries[i], (BTreeNode::numEntries - i) * sizeof(KeyTID));
            KeyTID keyTID;
            keyTID.key = key;
            keyTID.tid = tid;
            entries[i] = keyTID;

            ++BTreeNode::numEntries;
        }

        bool lookup(KeyT key, TID &tid) {
            for (uint16_t i = 0; i < BTreeNode::numEntries; ++i) {
                if (memcmp(&key, &entries[i].key, sizeof(KeyT)) == 0) {
                    tid = entries[i].tid;
                    return true;
                }
            }
            return false;
        }

        void split(BTreeLeaf *right, uint64_t rightID) {
            right->rightNeighbor = rightNeighbor; //Bplus-Tree
            rightNeighbor = rightID;

            uint16_t numEntriesLeft = BTreeNode::numEntries / (uint16_t) 2;
            right->numEntries = BTreeNode::numEntries - numEntriesLeft;

            memcpy(&right->entries[0], &entries[numEntriesLeft], right->numEntries * sizeof(KeyTID));
            BTreeNode::numEntries = numEntriesLeft;
        }

        KeyT getHighestKey() {
            return entries[BTreeNode::numEntries - 1].key;
        }
    };


public:
    BTree(BufferManager &bm, uint64_t segmentID) : bufferManager(bm), segmentID(segmentID << 48), rootPageID(0),
                                                   numPages(1) {
        _size = 0;
        BufferFrame *bufferFrame = bufferManager.fixPage(createID(rootPageID), true);
        BTreeNode *node = (BTreeNode *) bufferFrame->getData();
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

    bool isFull(BTreeNode *node) {
        if (node->isLeaf) return ((BTreeLeaf *) node)->isFull();
        else return ((BTreeInner *) node)->isFull();
    }

    void split(BTreeNode *node, BTreeNode *right, uint64_t rightID) {
        right->isLeaf = node->isLeaf;
        if (node->isLeaf) ((BTreeLeaf *) node)->split(((BTreeLeaf *) right), rightID);
        else ((BTreeInner *) node)->split(((BTreeInner *) right));
    }

    KeyT getHighestKey(BTreeNode *node) {
        if (node->isLeaf) return ((BTreeLeaf *) node)->getHighestKey();
        else return ((BTreeInner *) node)->getHighestKey();
    }

};

template<class KeyT, class CompT>
void BTree<KeyT, CompT>::insert(KeyT key, TID &tid) {

    BufferFrame *bufferFrameCurrent = bufferManager.fixPage(createID(rootPageID), true);
    BTreeNode *current = (BTreeNode *) bufferFrameCurrent->getData();

    if (isFull(current)) {
        uint64_t rightPageID = numPages;
        ++numPages;
        BufferFrame *bufferFrameRight = bufferManager.fixPage(createID(rightPageID), true);
        BTreeNode *right = (BTreeNode *) bufferFrameRight->getData();

        split(current, right, rightPageID);

        BufferFrame *bufferFrameNewRoot = bufferManager.fixPage(createID(numPages), true);
        BTreeInner *newRoot = (BTreeInner *) bufferFrameNewRoot->getData();
        newRoot->isLeaf = false;
        newRoot->numEntries = 0;

        KeyT leftKey = getHighestKey(current);
        KeyT rightKey = getHighestKey(right);

        newRoot->insert(leftKey, rootPageID);
        newRoot->insert(rightKey, rightPageID);

        rootPageID = numPages;
        ++numPages;

        bufferManager.unfixPage(bufferFrameCurrent, true);
        bufferManager.unfixPage(bufferFrameRight, true);

        bufferFrameCurrent = bufferFrameNewRoot;
        current = (BTreeNode *) newRoot;
    }

    for (; ;) {
        if (current->isLeaf) break;

        BTreeInner *currentInner = (BTreeInner *) current;
        uint64_t childPageID = currentInner->getChildID(key);

        BufferFrame *bufferFrameChild = bufferManager.fixPage(createID(childPageID), true);
        BTreeNode *child = (BTreeNode *) bufferFrameChild->getData();

        bool hasBeenSplit = false;
        if (isFull(child)) {
            uint64_t rightPageID = numPages;
            ++numPages;
            BufferFrame *bufferFrameRight = bufferManager.fixPage(createID(rightPageID), true);
            BTreeNode *right = (BTreeNode *) bufferFrameRight->getData();

            split(child, right, rightPageID);

            KeyT leftKey = getHighestKey(child);
            KeyT rightKey = getHighestKey(right);
            currentInner->updateKeyOfPageID(leftKey,
                                            childPageID); //link to left page already exists in parent node -> just update it
            currentInner->insert(rightKey, rightPageID);
            hasBeenSplit = true;

            bufferManager.unfixPage(bufferFrameRight, true);
        }

        if (hasBeenSplit) {
            bufferManager.unfixPage(bufferFrameChild, true);
            continue;
        } //start over with current

        bufferManager.unfixPage(bufferFrameCurrent, hasBeenSplit);
        bufferFrameCurrent = bufferFrameChild;
        current = child;
    }

    //We always enter a non-full leaf
    BTreeLeaf *leaf = (BTreeLeaf *) current;

    leaf->insert(key, tid);
    ++_size;
    bufferManager.unfixPage(bufferFrameCurrent, true);
}

template<class KeyT, class CompT>
bool BTree<KeyT, CompT>::erase(KeyT key) {
    BufferFrame *bufferFrameCurrent = bufferManager.fixPage(createID(rootPageID), true);
    BTreeNode *current = (BTreeNode *) bufferFrameCurrent->getData();

    for (; ;) {
        if (current->isLeaf) break;

        BTreeInner *currentInner = (BTreeInner *) current;
        uint64_t childPageID = currentInner->getChildID(key);

        BufferFrame *bufferFrameChild = bufferManager.fixPage(createID(childPageID), true);
        BTreeNode *child = (BTreeNode *) bufferFrameChild->getData();

        // swap current and child
        bufferManager.unfixPage(bufferFrameCurrent, false);
        bufferFrameCurrent = bufferFrameChild;
        current = child;
    }

    BTreeLeaf *leaf = (BTreeLeaf *) current;
    bool erased = leaf->erase(key);
    if (erased) --_size;

    bufferManager.unfixPage(bufferFrameCurrent, erased);
    return erased;
}

template<class KeyT, class CompT>
bool BTree<KeyT, CompT>::lookup(KeyT key, TID &tid) {
    BufferFrame *bufferFrameCurrent = bufferManager.fixPage(createID(rootPageID), false);
    BTreeNode *current = (BTreeNode *) bufferFrameCurrent->getData();

    for (; ;) {
        if (current->isLeaf) break;

        BTreeInner *currentInner = (BTreeInner *) current;
        uint64_t childPageID = currentInner->getChildID(key);

        BufferFrame *bufferFrameChild = bufferManager.fixPage(createID(childPageID), false);
        BTreeNode *child = (BTreeNode *) bufferFrameChild->getData();

        // swap current and child
        bufferManager.unfixPage(bufferFrameCurrent, false);
        bufferFrameCurrent = bufferFrameChild;
        current = child;
    }

    BTreeLeaf *leaf = (BTreeLeaf *) current;
    bool lookup_successful = leaf->lookup(key, tid);
    bufferManager.unfixPage(bufferFrameCurrent, false);
    return lookup_successful;
}


#endif //PROJECT_BTREE_H
