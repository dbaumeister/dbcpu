//
// Created by dbaumeister on 15.06.15.
//

#ifndef PROJECT_REGISTER_H
#define PROJECT_REGISTER_H

#include <stdlib.h>
#include <string>
#include <string.h>
#include <assert.h>

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

    int getInteger() {
        return *(int*)value;
    }

    void setInteger(int intVal) {
        *(int*)value = intVal;
    }

    std::string getString() {

        return *(std::string*) value;
    }

    void setString(const std::string& strVal){
        memcpy(value, &strVal, len);
    }

    unsigned  getType(){
        return type;
    }

private:
    unsigned type;
    void* value;


};


#endif //PROJECT_REGISTER_H
