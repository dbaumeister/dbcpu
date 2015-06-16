/*
 * Assignment 3 - provided code
 */

#ifndef PROJECT_RECORD_H
#define PROJECT_RECORD_H

#include <cstring>
#include <cstdlib>


// A simple Record implementation
class Record {
   unsigned len;
   char* data;

public:
   // Assignment Operator: deleted
   Record& operator=(Record& rhs) = delete;
   // Copy Constructor: deleted
   Record(Record& t) = delete;
   // Move Constructor
   Record(Record&& t);
   // Constructor
   Record(unsigned len, const char* const ptr);
   // Destructor
   ~Record();
   // Get pointer to data
   const char* getData() const;
   // Get data size in bytes
   unsigned getLen() const;
};

#endif //PROJECT_RECORD_H