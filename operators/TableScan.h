//
// Created by dbaumeister on 15.06.15.
//

#ifndef PROJECT_TABLESCAN_H
#define PROJECT_TABLESCAN_H

#include "../schema/Schema.h"
#include "Register.h"

class TableScan {
public:
    TableScan(Schema::Relation relation) {
        //TODO
    }

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

};


#endif //PROJECT_TABLESCAN_H
