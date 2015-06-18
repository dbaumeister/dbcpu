//
// Created by dbaumeister on 17.06.15.
//

#ifndef PROJECT_PROJECTIONOPERATOR_H
#define PROJECT_PROJECTIONOPERATOR_H

#include "Operator.h"

class ProjectionOperator : public Operator{
public:
    ProjectionOperator() = delete;

    ProjectionOperator(const std::vector<unsigned>& projectionIndices, Operator& child)
            : child(child), projectionIndices(projectionIndices){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    std::vector<Register*> registers;
    Operator& child;
    const std::vector<unsigned>& projectionIndices;

};

#endif //PROJECT_PROJECTIONOPERATOR_H
