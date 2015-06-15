cmake_minimum_required(VERSION 3.1)

find_package (Threads)

#set(SOURCE_FILES testing/slottedTest.cpp)
set(SOURCE_FILES main.cpp)

add_executable(mydbms ${SOURCE_FILES} buffer/BufferManager.h buffer/BufferManager.cpp buffer/BufferFrame.h buffer/BufferFrame.cpp buffer/replacement_strategy/ReplacementStrategy.h buffer/replacement_strategy/ReplacementStrategy.cpp buffer/disk_io/DiskIO.h buffer/disk_io/DiskIO.cpp slottedpages/Parser.h slottedpages/Parser.cpp slottedpages/Record.h slottedpages/segment/Schema.h slottedpages/segment/Schema.cpp slottedpages/Types.h slottedpages/segment/SPSegment.h slottedpages/segment/SPSegment.cpp slottedpages/Record.cpp slottedpages/segment/SlottedPage.h slottedpages/segment/SlottedPage.cpp slottedpages/segment/TID.h slottedpages/segment/SchemaIO.h slottedpages/segment/SchemaIO.cpp btree/BTree.h operators/Register.h operators/Register.cpp)
target_link_libraries (mydbms ${CMAKE_THREAD_LIBS_INIT})
add_definitions(-std=c++11)