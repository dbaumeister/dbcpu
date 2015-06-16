//
// Created by dbaumeister on 15.06.15.
//

#ifndef PROJECT_TABLESCAN_H
#define PROJECT_TABLESCAN_H

#include "Register.h"
#include "Operator.h"
#include "../relation/Relation.h"

class TableScan : public Operator{
public:
    TableScan(const Relation& relation) : relation(relation), index(0), registers(relation.numAttr){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    const Relation& relation;
    unsigned index;
    std::vector<Register*> registers;

    void fillRegister(int i);
    void initRegister(int i);

};


#endif //PROJECT_TABLESCAN_H
