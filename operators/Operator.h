//
// Created by dbaumeister on 16.06.15.
//

#ifndef PROJECT_OPERATOR_H
#define PROJECT_OPERATOR_H

#include <vector>
#include "Register.h"

class Operator {

    virtual void open() = 0;

    virtual bool next() = 0;

    virtual std::vector<Register*> getOutput() = 0;

    virtual void close() = 0;

};

#endif //PROJECT_OPERATOR_H
