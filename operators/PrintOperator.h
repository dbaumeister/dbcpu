//
// Created by dbaumeister on 16.06.15.
//

#ifndef PROJECT_PRINTOPERATOR_H
#define PROJECT_PRINTOPERATOR_H

#include "Operator.h"

class PrintOperator : public Operator {
public:

    PrintOperator() = delete;

    PrintOperator(std::ostream& stream, Operator& child) : stream(stream), child(child){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    std::ostream& stream;
    Operator& child;
};

#endif //PROJECT_PRINTOPERATOR_H
