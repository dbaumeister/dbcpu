#include <stdio.h>
#include <iostream>
#include <sstream>

#include "buffer/BufferManager.h"
#include "slottedpages/SPSegment.h"
#include "operators/Register.h"

void printArguments(int argc, const char* argv[]);
void testSlottedPageInsertWithRandomInserts(SlottedPage& sp);
void testSlottedPageRemove(SlottedPage& sp);
void testSlottedPage();
void testSPSegment();

void testRegisters();
void testRegisterEqualComparison();
void testRegisterLessThanComparison();

std::string intToString(int i) {
    std::stringstream ss;
    ss << i;
    std::string str(ss.str());
    return str;
}

int main(int argc, const char* argv[])
{

    testRegisters();
    testRegisterEqualComparison();
    testRegisterLessThanComparison();

    StringRegister r1;
    StringRegister r2;
    IntegerRegister r3;

    std::vector<Register*> registers;
    registers.push_back(&r1);
    registers.push_back(&r2);
    registers.push_back(&r3);

    return 0;
}


void testRegisterLessThanComparison(){
    IntegerRegister integerRegisterA;
    IntegerRegister integerRegisterB;

    for(int i = 0; i < 1000; ++i){

        integerRegisterA.setInteger(i);
        integerRegisterB.setInteger(i + 1);

        assert(integerRegisterA < integerRegisterB);
        assert(! (integerRegisterB < integerRegisterA));
    }

    const unsigned stringsize = 20;
    StringRegister stringRegisterA;
    StringRegister stringRegisterB;

    for(int i = 0; i < 8; ++i){ //Keep in mind: "9" > "10" (lexicographic sorting)
        std::string strA = intToString(i);
        assert(strA.size() < stringsize);


        std::string strB = intToString(i + 1);
        assert(strB.size() < stringsize);

        stringRegisterA.setString(strA);
        stringRegisterB.setString(strB);

        assert(stringRegisterA < stringRegisterB);
        assert(!(stringRegisterB < stringRegisterA));
    }
}

void testRegisterEqualComparison(){

    IntegerRegister integerRegisterA;
    IntegerRegister integerRegisterB;

    for(int i = 0; i < 1000; ++i){

        integerRegisterA.setInteger(i);
        integerRegisterB.setInteger(i);

        assert(integerRegisterA == integerRegisterB);
        assert(integerRegisterB == integerRegisterA);


        integerRegisterB.setInteger(i + 1);

        assert( ! (integerRegisterA == integerRegisterB));
        assert( ! (integerRegisterB == integerRegisterA));
    }

    const unsigned stringsize = 20;
    StringRegister stringRegisterA;
    StringRegister stringRegisterB;

    for(int i = 0; i < 1000; ++i){
        std::string str = intToString(i);
        assert(str.size() < stringsize);

        stringRegisterA.setString(str);
        stringRegisterB.setString(str);

        assert(stringRegisterA == stringRegisterB);
        assert(stringRegisterB == stringRegisterA);


        std::string strB = intToString(i + 1);
        assert(strB.size() < stringsize);

        stringRegisterB.setString((strB));

        assert( !(stringRegisterA == stringRegisterB));
        assert( !(stringRegisterB == stringRegisterA));
    }
}

void testRegisters(){
    IntegerRegister integerRegister;
    assert(integerRegister.getType() == INTEGER_REGISTER);
    for(int i = 0; i < 1000 * 1000; ++i){
        integerRegister.setInteger(i);
        int j = integerRegister.getInteger();
        assert(i == j);
    }

    const unsigned stringsize = 20;
    StringRegister stringRegister;
    assert(stringRegister.getType() == STRING_REGISTER);
    for(int i = 0; i < 1000; ++i){
        std::string str = intToString(i);
        assert(str.size() < stringsize);
        stringRegister.setString(str);
        std::string j = stringRegister.getString();
        assert(str == j);

    }
}


void printArguments(int argc, const char* argv[]){
    printf("argc: %d\n", argc);
    for(int i = 0; i < argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);

    }
}