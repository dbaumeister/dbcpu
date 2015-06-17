//
// Created by dbaumeister on 17.06.15.
//

#include "ProjectionOperator.h"

void ProjectionOperator::open() {
    child.open();
}

void ProjectionOperator::close() {
    child.close();
}

bool ProjectionOperator::next() {
    if(child.next()){
        registers.clear();

        std::vector<Register*> r = child.getOutput();
        for(int i = 0; i < projectionIndices.size(); ++i){
            unsigned index = projectionIndices[i];

            if(index >= r.size()) return false;
            else registers.push_back(r[index]);

        }
        return true;
    } else return false;
}

std::vector<Register *> ProjectionOperator::getOutput() {
    return registers;
}
