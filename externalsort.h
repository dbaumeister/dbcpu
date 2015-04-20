//
// Created by dbaumeister on 18.04.15.
//

#ifndef PROJECT_EXTERNALSORT_H
#define PROJECT_EXTERNALSORT_H

#include <stdint.h>

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize);
bool isFileSorted(int fd, uint64_t size, uint64_t memSize);


#endif //PROJECT_EXTERNALSORT_H
