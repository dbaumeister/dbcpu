/*
 * Assignment 3 - provided code
 */


#ifndef PROJECT_PARSER_H
#define PROJECT_PARSER_H

#include <exception>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include "segment/Schema.h"

class ParserError : std::exception {
   std::string msg;
   unsigned line;
   public:
   ParserError(unsigned line, const std::string& m) : msg(m), line(line) {}
   ~ParserError() throw() {}
   const char* what() const throw() {
      return msg.c_str();
   }
};

struct Parser {
   std::string fileName;
   std::ifstream in;
   enum class State : unsigned { Init, Create, Table, CreateTableBegin, CreateTableEnd, TableName, Primary, Key, KeyListBegin, KeyName, KeyListEnd, AttributeName, AttributeTypeInt, AttributeTypeChar, CharBegin, CharValue, CharEnd, AttributeTypeNumeric, NumericBegin, NumericValue1, NumericSeparator, NumericValue2, NumericEnd, Not, Null, Separator, Semicolon};
   State state;
   Parser(const std::string& fileName) : fileName(fileName), state(State::Init) {}
   ~Parser() {};
   std::unique_ptr<Schema> parse();

   private:
   void nextToken(unsigned line, const std::string& token, Schema& s);
};

#endif //PROJECT_PARSER_H
