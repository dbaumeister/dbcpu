//
// Created by dbaumeister on 05.05.15.
//

#include "SPSegment.h"


TID SPSegment::insert(Record &record) {
    // find page with enough space
    for(uint16_t i = 0; i < slottedPageCount; ++i){
        BufferFrame* bufferFrame = bufferManager.fixPage(createID(i), true);
        SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

        if(sp->getNumUnusedSlots() > 0) {
            std::cout << "There are removed slots on page " << i << "-> try to insert in one of these." << std::endl;
            uint16_t slotID = sp->getFirstUnusedSlot();
            if(sp->isValid(slotID)){
                if(sp->tryUpdateRemovedSlot(slotID, record.getData(), (uint16_t) record.getLen())){
                    //inserting in slot was successful
                    TID tid;
                    tid.slotID = slotID;
                    tid.pageID = i;
                    bufferManager.unfixPage(bufferFrame, true);
                    return tid;
                } else std::cout << "Could not insert in removed slot - there is no space!" << std::endl;
            } else std::cout << "Could not insert in remmoved slot - it was invalid" << std::endl;
        }

        if(sp->hasEnoughSpace((uint16_t) record.getLen())){
            std::cout << "Insert because we have enough space on page " << i << "." << std::endl;

            TID tid;
            tid.slotID = sp->insertNewSlot(record.getData(), (uint16_t)record.getLen());
            tid.pageID = i;
            bufferManager.unfixPage(bufferFrame, true);

            return tid;
        }
        else if(sp->hasEnoughSpaceAfterDefrag((uint16_t)record.getLen())){ //no free space found ->search again for fragmented
            std::cout << "Insert because we have enough space after defrag on page " << i << "." << std::endl;

            sp->defrag();
            TID tid;
            tid.slotID = sp->insertNewSlot(record.getData(), (uint16_t)record.getLen());
            tid.pageID = i;
            bufferManager.unfixPage(bufferFrame, true);
            return tid;
        }
        bufferManager.unfixPage(bufferFrame, false);
    }

    std::cout << "Insert New Page in Segment" << std::endl;
    // If not found -> create a new page (with BufferManager.fixPage)
    BufferFrame* bufferFrame = bufferManager.fixPage(createID(slottedPageCount), true);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    sp->header.fragmentedSpace = 0;
    sp->header.numUnusedSlots = 0;
    sp->header.slotCount = 0;
    sp->header.dataStart = PAGESIZE - sizeof(SlottedPage::SPHeader);

    TID tid;
    tid.slotID = sp->insertNewSlot(record.getData(), (uint16_t)record.getLen());
    tid.pageID = slottedPageCount;

    ++slottedPageCount;
    bufferManager.unfixPage(bufferFrame, true);

    return tid;
}

bool SPSegment::remove(TID tid) {

    BufferFrame* bufferFrame = bufferManager.fixPage(createID(tid.pageID), true);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    //check if the slot is valid - only continue if it is
    if(! sp->isValid(tid.slotID)) {
        bufferManager.unfixPage(bufferFrame, false);
        return false;
    }

    //check for indirection
    if(sp->isIndirection(tid.slotID)){
        //delete indirection
        std::cout << "Remove points to indirection -> ";
        TID indirectionTID = sp->getIndirection(tid.slotID);
        return remove(indirectionTID);
    }

    std::cout << "Remove slot" << std::endl;
    //remove whatever now lies in this slot
    sp->remove(tid.slotID);
    bufferManager.unfixPage(bufferFrame, true);
    return true;
}

// updates only non removed tids
bool SPSegment::update(TID tid, Record &record) {
    BufferFrame* bufferFrame = bufferManager.fixPage(createID(tid.pageID), true);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    if(!sp->isValid(tid.slotID)) {
        printf("SlotID of TID not valid. (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        throw new std::invalid_argument("SlotID of TID not valid.");
    }

    else if(sp->isRemoved(tid.slotID)){
        printf("SlotID of TID has already been removed. (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        throw new std::invalid_argument("Tried to update removed slot.");
    }

    else if(sp->isIndirection(tid.slotID)){
        std::cout << "Update Slot with Indirection in it" << std::endl;
        TID indirectionTID = sp->getIndirection(tid.slotID);
        bool success = sp->tryUpdateSlotWithIndirection(tid.slotID, record.getData(), (uint16_t) record.getLen());
        if(success){
            std::cout << "Update Slot with Indirection - direct update - success" << std::endl;

            if(remove(indirectionTID)){
                std::cout << "Could remove indirectionTID" << std::endl;
            } else std::cout << "Could NOT remove indirectionTID" << std::endl;

            bufferManager.unfixPage(bufferFrame, true);
            return true;
        } else {
            std::cout << "Update Slot with Indirection - fail - try to fit the update into the indirection" << std::endl;
            //try to fit the update into the indirection
            BufferFrame* bufferFrameIndir = bufferManager.fixPage(createID(indirectionTID.pageID), true);
            SlottedPage* spIndir = (SlottedPage*) bufferFrameIndir->getData();

            success = spIndir->tryUpdate(indirectionTID.slotID, record.getData(), (uint16_t)record.getLen());
            if(success) {
                std::cout << "Success - could fit the update into the indirection" << std::endl;
                bufferManager.unfixPage(bufferFrameIndir, true);
                bufferManager.unfixPage(bufferFrame, false);
                return true;
            }
            else {
                std::cout << "Could not fit update into indirection - remove indirection and insert update" << std::endl;
                spIndir->remove(indirectionTID.slotID);
                bufferManager.unfixPage(bufferFrameIndir, true);
                bufferManager.unfixPage(bufferFrame, false);

                indirectionTID = insert(record);


                bufferFrame = bufferManager.fixPage(createID(tid.pageID), true);
                sp = (SlottedPage*) bufferFrame->getData();

                sp->insertIndirection(tid.slotID, indirectionTID);
                bufferManager.unfixPage(bufferFrame, true);
                return true;
            }
        }
    }
    std::cout << "Update regular slot" << std::endl;
    //no indirection and not removed -> update here
    bool success = sp->tryUpdate(tid.slotID,  record.getData(), (uint16_t) record.getLen());
    if(success){
        std::cout << "Update regular slot - success" << std::endl;
        bufferManager.unfixPage(bufferFrame, true);
        return true;
    } else {
        std::cout << "Update regular slot - fail - must insert indirection" << std::endl;

        bufferManager.unfixPage(bufferFrame, false);
        TID indirectionTID = insert(record);

        bufferFrame = bufferManager.fixPage(createID(tid.pageID), true);
        sp = (SlottedPage*) bufferFrame->getData();

        sp->remove(tid.slotID);
        sp->insertIndirection(tid.slotID, indirectionTID);
        bufferManager.unfixPage(bufferFrame, true);
        return true;
    }
}

/*
 * Attention!
 * Returned value has to be freed!
 */
Record &SPSegment::lookup(TID tid) {

    BufferFrame* bufferFrame = bufferManager.fixPage(createID(tid.pageID), false);
    SlottedPage* sp = (SlottedPage*) bufferFrame->getData();

    if(!sp->isValid(tid.slotID)){
        printf("SlotID of TID not valid. (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        throw new std::invalid_argument("SlotID of TID not valid.");
    }
    else if(sp->isRemoved(tid.slotID)){
        printf("Lookup of removed TID (PageID: %lu, SlotID: %u)\n", tid.pageID, tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        throw new std::invalid_argument("Lookup of removed TID.");
    }
    else if(sp->isIndirection(tid.slotID)){
        TID indirectionTID = sp->getIndirection(tid.slotID);
        Record* r = &lookup(indirectionTID);
        bufferManager.unfixPage(bufferFrame, false);
        return *r;
    }
    else {
        Record* r = &sp->getRecordFromSlotID(tid.slotID);
        bufferManager.unfixPage(bufferFrame, false);
        return *r;
    }

}

uint64_t SPSegment::createID(uint64_t pageID) {
    return segIDShifted + pageID;
}
