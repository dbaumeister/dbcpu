//
// Created by dbaumeister on 15.06.15.
//

#ifndef PROJECT_TABLESCAN_H
#define PROJECT_TABLESCAN_H

#include "Register.h"
#include "Operator.h"
#include "../slottedpages/SPSegment.h"
#include "../Tuple.h"


class TableScanOperator : public Operator{
public:
    TableScanOperator() = delete;

    TableScanOperator(const SPSegment& segment) : segment(segment){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    const SPSegment& segment;
    TID currentTID;
    std::vector<Register*> registers;

    void fillRegisters(const Tuple& tuple);

};


#endif //PROJECT_TABLESCAN_H
