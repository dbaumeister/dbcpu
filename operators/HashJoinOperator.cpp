//
// Created by dbaumeister on 17.06.15.
//

#include "HashJoinOperator.h"

void HashJoinOperator::open() {
    hashtable.clear();
    opLeft.open();
    while(opLeft.next()){
        std::vector<Register*> r = opLeft.getOutput();
        insertInHashtable(r[leftID]->getHashValue(), r);
    }
    opLeft.close();
    opRight.open();
}


void HashJoinOperator::insertInHashtable(size_t key, const std::vector<Register *>& value) {
    hashtable.insert(std::pair<size_t, std::vector<Register*>>(key, value));
}

void HashJoinOperator::close() {
    opRight.close();
}

bool HashJoinOperator::next() {
    if(opRight.next()){
        std::vector<Register*> rightRegisters = opRight.getOutput();
        auto it = hashtable.find(rightRegisters[rightID]->getHashValue());

        if(it == hashtable.end()){
            return next();
        }

        registers.clear();
        for(Register* leftR : it->second){
            registers.push_back(leftR);
        }
        for(Register* rightR : rightRegisters){
            registers.push_back(rightR);
        }
        return true;
    }
    else return false;
}

std::vector<Register *> HashJoinOperator::getOutput() {
    return registers;
}

