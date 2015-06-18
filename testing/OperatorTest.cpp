//
// Created by dbaumeister on 18.06.15.
//


#include <stdio.h>
#include <iostream>
#include <sstream>

#include "../buffer/BufferManager.h"
#include "../slottedpages/SPSegment.h"
#include "../operators/Register.h"
#include "../operators/Operator.h"
#include "../operators/PrintOperator.h"
#include "../operators/SelectionOperator.h"

class TestOperator : public Operator {
public:

    TestOperator(unsigned maxTuples) : maxTuples(maxTuples), index(0) {

        StringRegister* r1 = new StringRegister();
        StringRegister* r2 = new StringRegister();
        IntegerRegister* r3 = new IntegerRegister();

        std::string vorname = "Herbert";
        std::string nachname = "Maier";
        int geburtsjahr = 1970;

        r1->setString(vorname);
        r2->setString(nachname);
        r3->setInteger(geburtsjahr);

        registers.push_back(r1);
        registers.push_back(r2);
        registers.push_back(r3);
    }

    ~TestOperator(){
        for(Register* r : registers){
            delete r;
        }
    }

    void open(){
        index = 0;
    }

    bool next(){
        if(index < maxTuples){
            index++;
            return true;
        }
        else return false;
    }

    std::vector<Register*> getOutput(){
        return registers;
    }

    void close(){}

    unsigned maxTuples, index;
    std::vector<Register*> registers;
};


void testRegisterSetGet();
void testRegisterEqualComparison();
void testRegisterLessThanComparison();


void testPrintOperator();
void testSelectionOperator();

std::string intToString(int i) {
    std::stringstream ss;
    ss << i;
    std::string str(ss.str());
    return str;
}

int main(int argc, const char* argv[])
{

    testRegisterSetGet();

    testRegisterEqualComparison();

    testRegisterLessThanComparison();

    testPrintOperator();

    testSelectionOperator();

    return 0;
}
void testSelectionOperator(){

}

void testPrintOperator(){
    std::cout << "testPrintOperator" << std::endl;
    TestOperator t(3);
    PrintOperator p(std::cout, t);
    p.open();
    while(p.next());
    p.close();
}


void testRegisterLessThanComparison(){

    std::cout << "testRegisterLessThanComparison" << std::endl;
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

    std::cout << "testRegisterEqualComparison" << std::endl;
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

void testRegisterSetGet(){
    std::cout << "testRegisterSetGet" << std::endl;
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

    StringRegister r1;
    StringRegister r2;
    IntegerRegister r3;

    std::string s1 = "Hello World";
    std::string s2 = "XYZ";
    int i3 = 3;
    r1.setString(s1);
    r2.setString(s2);
    r3.setInteger(i3);

    std::vector<Register*> registers;
    registers.push_back(&r1);
    registers.push_back(&r2);
    registers.push_back(&r3);

    assert(r1.getString() == s1);
    assert(r2.getString() == s2);
    assert(r3.getInteger() == i3);
}