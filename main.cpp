#include <stdio.h>
#include <iostream>
#include <sstream>

#include "buffer/BufferManager.h"
#include "slottedpages/segment/SPSegment.h"
#include "operators/Register.h"

void printArguments(int argc, const char* argv[]);
void testSlottedPageInsertWithRandomInserts(SlottedPage& sp);
void testSlottedPageRemove(SlottedPage& sp);
void testSlottedPage();
void testSPSegment();

void testRegisters();

std::string intToString(int i) {
    std::stringstream ss;
    ss << i;
    std::string str(ss.str());
    return str;
}

int main(int argc, const char* argv[])
{
    testRegisters();

    return 0;
}


void testRegisters(){
    Register<sizeof(int)> integerRegister(INTEGER_REGISTER);
    assert(integerRegister.getType() == INTEGER_REGISTER);
    for(int i = 0; i < 1000 * 1000; ++i){
        integerRegister.setInteger(i);
        int j = integerRegister.getInteger();
        assert(i == j);
    }

    const unsigned stringsize = 20;
    Register<stringsize * sizeof(char)> stringRegister(STRING_REGISTER);
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