//
// Created by dbaumeister on 17.06.15.
//

#include "SelectionOperator.h"

void SelectionOperator::open() {
    child.open();
}

void SelectionOperator::close() {
    child.next();
}

bool SelectionOperator::next() {
    if(child.next()){
        registers.clear();
        std::vector<Register*> r = child.getOutput();
        if(*r[id] == comparisonRegister){
            registers = r; //TODO check if works
            return true;
        } else return false;
    } else return false;
}

std::vector<Register *> SelectionOperator::getOutput() {
    return registers;
}
