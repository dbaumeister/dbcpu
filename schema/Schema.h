/*
 * Assignment 3 - provided code
 */


#ifndef PROJECT_SCHEMA_H
#define PROJECT_SCHEMA_H

#include <vector>
#include <string>
#include "Types.h"

struct Schema {
    struct Relation {
        struct Attribute {
            std::string name;
            Types::Tag type;
            unsigned len;
            bool notNull;

            Attribute() : len(~0), notNull(true) { }
        };

        std::string name;
        std::vector<Schema::Relation::Attribute> attributes;
        std::vector<unsigned> primaryKey;

        Relation(const std::string &name) : name(name) { }
    };

    std::vector<Schema::Relation> relations;

    std::string toString() const;
};

#endif //PROJECT_SCHEMA_H
