//
// Created by dbaumeister on 15.06.15.
//

#include "TableScanOperator.h"

void TableScan::fillRegister(int i){
    Attribute* a = relation.tuples[index]->attributes[i];
    if(a->getType() == STRINGTYPE){
        registers[i]->setString(a->getString());
    }
    else if (a->getType() == INTEGERTYPE){
        registers[i]->setInteger(a->getInteger());
    }
    else {
        std::cerr << "Attribute-Type not supported!" << std::endl;
        exit(1);
    }
}

bool TableScan::next() {
    if(index < relation.tuples.size()) {
        for(int i = 0; i < relation.numAttr; ++i){
            fillRegister(i);
        }
        index++;
        return true;
    }
    else return false;
}

std::vector<Register *> TableScan::getOutput() {
    return registers;
}

void TableScan::initRegister(int i) {
    Attribute* a = relation.tuples[0]->attributes[i];
    if(a->getType() == STRINGTYPE){
        registers[i] = new StringRegister();
    }
    else if (a->getType() == INTEGERTYPE){
        registers[i] = new IntegerRegister();
    }
    else {
        std::cerr << "Attribute-Type not supported!" << std::endl;
        exit(1);
    }
}

void TableScan::open() {
    index = 0;
    if (relation.tuples.size() > 0) {
        for(int i = 0; i < relation.numAttr; ++i){
            initRegister(i);
        }
    }
}

void TableScan::close() {
    if (relation.tuples.size() > 0) {
        for(Register* r : registers){
            free(r);
        }
    }
}


