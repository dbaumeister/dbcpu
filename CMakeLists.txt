cmake_minimum_required(VERSION 3.1)

find_package (Threads)

set(SOURCE_FILES testing/buffertest.cpp)
add_executable(mydbms ${SOURCE_FILES} buffer/BufferManager.h buffer/BufferManager.cpp buffer/BufferFrame.h buffer/BufferFrame.cpp buffer/replacement_strategy/ReplacementStrategy.h buffer/replacement_strategy/ReplacementStrategy.cpp buffer/disk_io/DiskIO.h buffer/disk_io/DiskIO.cpp slottedpages/segment/SPSegment.h slottedpages/segment/SPSegment.cpp)
target_link_libraries (mydbms ${CMAKE_THREAD_LIBS_INIT})
add_definitions(-std=c++11)