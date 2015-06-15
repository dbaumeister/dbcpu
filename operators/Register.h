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
template <unsigned len>
class Register {
public:
    Register(unsigned type) : type(type){
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

    int getHashValue(){
        if(type == STRING_REGISTER){
            return atoi(getString().c_str()); //TODO evaluate usability of this hash function
        }
        else if(type == INTEGER_REGISTER){
            return getInteger();
        }
    }

private:
    unsigned type;
    void* value;


};



template <unsigned len>
bool operator ==(const Register<len>& left, const Register<len>& right){

    if(left.getType() == right.getType()){ //same type needed
        if(left.getType() == STRING_REGISTER) return left.getString() == right.getString();
        else if(left.getType() == INTEGER_REGISTER) return left.getInteger() == right.getInteger();
        else return false;

    } else
        return false;

}

template <unsigned len>
bool operator <(const Register<len>& left, const Register<len>& right){
    assert(left.getType() == right.getType()); //same type needed

    if(left.getType() == STRING_REGISTER) return left.getString() < right.getString();
    else if(left.getType() == INTEGER_REGISTER) return left.getInteger() < right.getInteger();
    else return false;
}


#endif //PROJECT_REGISTER_H
