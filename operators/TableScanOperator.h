//
// Created by dbaumeister on 15.06.15.
//

#ifndef PROJECT_TABLESCAN_H
#define PROJECT_TABLESCAN_H

#include "Register.h"
#include "Operator.h"
#include "../slottedpages/SPSegment.h"
#include "../Tuple.h"


class TableScan : public Operator{
public:
    TableScan(SPSegment* segment) : segment(segment){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    SPSegment* segment;
    TID currentTID;
    std::vector<Register*> registers;

    void fillRegisters(const Tuple& tuple);

};


#endif //PROJECT_TABLESCAN_H
