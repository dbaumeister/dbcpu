//
// Created by dbaumeister on 15.06.15.
//

#include "TableScanOperator.h"

void TableScanOperator::fillRegisters(const Tuple& tuple){
    registers[0]->setInteger(tuple.a);
    registers[1]->setInteger(tuple.b);
    registers[2]->setInteger(tuple.c);
    registers[3]->setString(std::string(tuple.d));
}

bool TableScanOperator::next() {
    if(currentTID.pageID < segment.getPageCount()){
        try {
            Tuple* tuple = (Tuple*)(void*)&segment.lookup(currentTID); //TODO evaluate if this very unpleasent cast works
            fillRegisters(*tuple);
            currentTID.slotID++;
            return true;

        }
        catch(int e) {
            if(e == SLOTID_OUT_OF_BOUNDS_EXCEPTION){
                currentTID.pageID++;
                return next();
            }
            else if(e == SLOTID_TO_REMOVED_SLOT_EXCEPTION){
                currentTID.slotID++;
                return next();
            }
            else return false;
        }
    }
    else return false;
}

std::vector<Register *> TableScanOperator::getOutput() {
    return registers;
}

void TableScanOperator::open() {
    currentTID.pageID = 0;
    currentTID.slotID = 0;
    //ATTENTION: this has to have the same structure as the Tuple struct in Tuple.h
    registers.push_back(new IntegerRegister());
    registers.push_back(new IntegerRegister());
    registers.push_back(new IntegerRegister());
    registers.push_back(new StringRegister());
}

void TableScanOperator::close() {
    for(int i = 0; i < registers.size(); ++i){
        delete registers[i];
    }
}