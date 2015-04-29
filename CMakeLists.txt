cmake_minimum_required(VERSION 3.1)

find_package (Threads)

set(SOURCE_FILES main.cpp)
add_executable(mydbms ${SOURCE_FILES} legacy/externalsort.h legacy/externalsort.cpp buffer/BufferManager.h buffer/BufferManager.cpp buffer/BufferFrame.h buffer/BufferFrame.cpp collection/LockedCollection.h buffer/replacement_strategy/ReplacementStrategy.h buffer/replacement_strategy/ReplacementStrategy.cpp exceptions/exceptions.h buffer/disk_io/DiskIO.h buffer/disk_io/DiskIO.cpp)
target_link_libraries (mydbms ${CMAKE_THREAD_LIBS_INIT})
add_definitions(-std=c++11)