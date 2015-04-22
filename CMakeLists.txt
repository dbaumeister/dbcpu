cmake_minimum_required(VERSION 3.1)

set(SOURCE_FILES main.cpp)
add_executable(mydbms ${SOURCE_FILES} legacy/externalsort.h legacy/externalsort.cpp buffer/BufferManager.h buffer/BufferManager.cpp buffer/BufferFrame.h buffer/BufferFrame.cpp collection/LockedCollection.h buffer/BufferFrameWrapper.h buffer/BufferFrameWrapper.cpp buffer/ReplacementStrategy.h buffer/ReplacementStrategy.cpp)
add_definitions(-std=c++11)