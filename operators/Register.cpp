//
// Created by dbaumeister on 15.06.15.
//

#include "Register.h"


bool operator ==(const Register& left, const Register& right){

    if(left.getType() == right.getType()){ //same type needed
        if(left.getType() == STRING_REGISTER) return left.getString() == right.getString();
        else if(left.getType() == INTEGER_REGISTER) return left.getInteger() == right.getInteger();
        else return false;

    } else
        return false;

}

bool operator <(const Register& left, const Register& right){
    assert(left.getType() == right.getType()); //same type needed

    if(left.getType() == STRING_REGISTER) return left.getString() < right.getString();
    else if(left.getType() == INTEGER_REGISTER) return left.getInteger() < right.getInteger();
    else return false;
}
