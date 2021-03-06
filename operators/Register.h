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
#include "../Tuple.h"

#define STRING_REGISTER 1
#define INTEGER_REGISTER 2



/*
 * Init with size in bytes as template parameter and type of register as constructor parameter
 */

class Register {
public:
    Register() = delete;

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
        std::string strVal((char*) value);
        return strVal;
    }

    void setString(const std::string& strVal){
        const char* c = strVal.c_str();
        memcpy(value, c, len);
    }

    unsigned  getType() const{
        return type;
    }

    size_t getHashValue(){
        if(type == STRING_REGISTER){
            return std::hash<std::string>()(getString());
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
    StringRegister() : Register(STRING_REGISTER, TUPLE_STRING_SIZE){}
};

class IntegerRegister : public Register {
public:
    IntegerRegister() : Register(INTEGER_REGISTER, sizeof(int)){}
};


bool operator ==(const Register& left, const Register& right);

bool operator <(const Register& left, const Register& right);


#endif //PROJECT_REGISTER_H
