//
// Created by dbaumeister on 17.06.15.
//

#ifndef PROJECT_HASHJOINOPERATOR_H
#define PROJECT_HASHJOINOPERATOR_H


#include <unordered_map>
#include "Operator.h"

class HashJoinOperator : public Operator{
public:

    HashJoinOperator(unsigned leftID, Operator& opLeft, unsigned rightID, Operator& opRight)
            : leftID(leftID), rightID(rightID), opLeft(opLeft), opRight(opRight){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    std::vector<Register*> registers;
    unsigned leftID;
    unsigned rightID;
    Operator& opLeft;
    Operator& opRight;

    void insertInHashtable(size_t key,const std::vector<Register*>& value);
    std::unordered_map<size_t, std::vector<Register*>> hashtable;

};

#endif //PROJECT_HASHJOINOPERATOR_H
