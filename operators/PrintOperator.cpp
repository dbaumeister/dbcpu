//
// Created by dbaumeister on 16.06.15.
//

#include "PrintOperator.h"

void PrintOperator::open() {
    child.open();
}

bool PrintOperator::next() {
    if(child.next()){
        std::vector<Register*> registers = child.getOutput();
        for(int i = 0; i < registers.size(); ++i){
            if(registers[i]->getType() == STRING_REGISTER){
                stream << registers[i]->getString();
            }
            else if(registers[i]->getType() == INTEGER_REGISTER){
                stream << registers[i]->getInteger();
            }
            else stream << "TYPE ERROR";

            stream << "; ";
        }
        stream << std::endl;
        return true;

    } else return false;
}

std::vector<Register *> PrintOperator::getOutput() {
    return std::vector<Register *>();
}

void PrintOperator::close() {
    child.close();
}
