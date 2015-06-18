//
// Created by dbaumeister on 17.06.15.
//

#ifndef PROJECT_SELECTIONOPERATOR_H
#define PROJECT_SELECTIONOPERATOR_H

#include "Operator.h"

class SelectionOperator : public Operator{
public:

    SelectionOperator() = delete;

    SelectionOperator(unsigned id, const Register& comparisonRegister, Operator& child)
            : id(id), comparisonRegister(comparisonRegister), child(child) {}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    unsigned id;
    Operator& child;
    const Register& comparisonRegister;
    std::vector<Register*> registers;

};

#endif //PROJECT_SELECTIONOPERATOR_H
