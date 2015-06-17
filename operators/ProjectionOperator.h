//
// Created by dbaumeister on 17.06.15.
//

#ifndef PROJECT_PROJECTIONOPERATOR_H
#define PROJECT_PROJECTIONOPERATOR_H

#include "Operator.h"

class ProjectionOperator : public Operator{
public:
    ProjectionOperator(const std::vector<int>& projectionIndices, Operator& child)
            : child(child), projectionIndices(projectionIndices){}

    void open();

    bool next();

    std::vector<Register*> getOutput();

    void close();

private:
    Operator& child;
    const std::vector<int>& projectionIndices;

};

#endif //PROJECT_PROJECTIONOPERATOR_H
