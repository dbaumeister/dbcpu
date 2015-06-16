//
// Created by dbaumeister on 15.06.15.
//

#ifndef PROJECT_REGISTER_H
#define PROJECT_REGISTER_H

#include <stdlib.h>
#include <string>
#include <string.h>
#include <assert.h>
#include <iostream>

#define STRING_REGISTER 1
#define INTEGER_REGISTER 2

/*
 * Init with size in bytes as template parameter and type of register as constructor parameter
 */

class Register {
public:
    Register(unsigned type, unsigned len) : type(type), len(len){
        bool isCorrectRegisterType = type == STRING_REGISTER || type == INTEGER_REGISTER;
        assert(isCorrectRegisterType);

        if(type == INTEGER_REGISTER){
            assert(len == sizeof(int));
        }

        value = malloc(len);
    }

    ~Register(){
        free(value);
    }

    int getInteger() const{
        return *(int*)value;
    }

    void setInteger(int intVal) {
        *(int*)value = intVal;
    }

    std::string getString() const{

        return *(std::string*) value;
    }

    void setString(const std::string& strVal){
        memcpy(value, &strVal, len);
    }

    unsigned  getType() const{
        return type;
    }

    size_t getHashValue(){
        if(type == STRING_REGISTER){
            return std::hash<std::string>()(getString()); //TODO evaluate usability of this hash function
        }
        else if(type == INTEGER_REGISTER){
            return (size_t)getInteger();
        }
    }

private:
    unsigned type, len;
    void* value;


};

class StringRegister : public Register {
public:
    StringRegister() : Register(STRING_REGISTER, 20){}
};

class IntegerRegister : public Register {
public:
    IntegerRegister() : Register(INTEGER_REGISTER, sizeof(int)){}
};


bool operator ==(const Register& left, const Register& right);

bool operator <(const Register& left, const Register& right);


#endif //PROJECT_REGISTER_H
