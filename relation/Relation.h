//
// Created by dbaumeister on 16.06.15.
//

#ifndef PROJECT_RELATION_H
#define PROJECT_RELATION_H

#include <vector>
#include <string>
#include <string.h>
#include <assert.h>

#define STRINGTYPE 1
#define INTEGERTYPE 2

class Attribute {
public:
    Attribute(const std::string& val) : type(STRINGTYPE), len(val.size()){

        value = malloc(len);
        memcpy(value, &val, len);

    }

    Attribute(int val) : type(INTEGERTYPE), len(sizeof(int)){

        value = malloc(len);
        *(int*)value = val;
    }

    ~Attribute(){
        free(value);
    }

    std::string getString() const{

        return *(std::string*) value;
    }

    int getInteger() const{
        return *(int*)value;
    }

    unsigned  getType() const{
        return type;
    }

private:
    unsigned type;
    size_t len;
    void* value;
};


class Tuple {
public:


    void addAttribute(Attribute* attr){
        attributes.push_back(attr);
    }

    std::vector<Attribute*> attributes;

};

/*
 * Dummy Wrapper to avoid conversion from record to relation at the moment
 */
class Relation {
public:

    Relation(std::string name, unsigned numAttributes, std::vector<std::string> attrNames, std::vector<unsigned> primaryKey)
            : name(name), primaryKey(primaryKey), attrNames(attrNames), numAttr(numAttributes) {
        assert(numAttributes == attrNames.size());
    }


    void addTuple(Tuple* tuple){
        tuples.push_back(tuple);
    }

    unsigned numAttr;
    std::string name;
    std::vector<Tuple*> tuples;
    std::vector<unsigned> primaryKey;
    std::vector<std::string> attrNames;
};


#endif //PROJECT_RELATION_H
