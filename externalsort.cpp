//
// Created by dbaumeister on 18.04.15.
//

#include <iostream>
#include <math.h>
#include <unistd.h>
#include <cstdlib>
#include <algorithm>

#include "externalsort.h"


void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize){
    std::cout << "size: " << size << std::endl << "memSize: " << memSize << std::endl;


    uint64_t numElementsPerSweep = memSize / sizeof(uint64_t); //integer flooring
    uint64_t sizePerSweep = numElementsPerSweep * sizeof(uint64_t);

    int numSweeps = size / sizePerSweep; //integer flooring
    std::cout << "Number of sweeps necessary: " << numSweeps << std::endl;


    int i = 0;
    for(; i < numSweeps; ++i){
        std::cout << "Sweep " << i << ": ";
        uint64_t* values = (uint64_t*) malloc(sizePerSweep);
        ssize_t sizeRead = pread(fdInput, values, sizePerSweep, i * sizePerSweep);
        std::cout << sizeRead << " bytes read." << std::endl;

        std::sort(values, values + sizeRead / sizeof(uint64_t));

        pwrite(fdOutput, values, sizeRead, i * sizePerSweep);

        free(values);
    }
    uint64_t rest = size - numSweeps * sizePerSweep;
    if(rest > 0) { //if there are leftovers
        std::cout << "Process the leftover bytes: ";
        uint64_t* values = (uint64_t*) malloc(rest);
        ssize_t sizeRead = pread(fdInput, values, rest, i * sizePerSweep);
        std::cout << sizeRead << " bytes read." << std::endl;

        std::sort(values, values + sizeRead / sizeof(uint64_t));

        pwrite(fdOutput, values, sizeRead, i * sizePerSweep);

        free(values);
    }

}