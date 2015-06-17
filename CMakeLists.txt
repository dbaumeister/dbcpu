cmake_minimum_required(VERSION 3.1)

find_package (Threads)

#set(SOURCE_FILES testing/slottedTest.cpp)
set(SOURCE_FILES main.cpp)

add_executable(mydbms ${SOURCE_FILES} buffer/BufferManager.h buffer/BufferManager.cpp buffer/BufferFrame.h buffer/BufferFrame.cpp buffer/replacement_strategy/ReplacementStrategy.h buffer/replacement_strategy/ReplacementStrategy.cpp buffer/disk_io/DiskIO.h buffer/disk_io/DiskIO.cpp schema/Parser.h schema/Parser.cpp schema/Record.h schema/Schema.h schema/Schema.cpp schema/Types.h slottedpages/SPSegment.h slottedpages/SPSegment.cpp schema/Record.cpp slottedpages/SlottedPage.h slottedpages/SlottedPage.cpp slottedpages/TID.h schema/SchemaIO.h schema/SchemaIO.cpp btree/BTree.h operators/Register.h operators/Register.cpp operators/TableScanOperator.h operators/TableScanOperator.cpp operators/Operator.h operators/PrintOperator.h operators/PrintOperator.cpp Tuple.h operators/ProjectionOperator.h operators/ProjectionOperator.cpp operators/SelectionOperator.h operators/SelectionOperator.cpp operators/HashJoinOperator.h operators/HashJoinOperator.cpp)
target_link_libraries (mydbms ${CMAKE_THREAD_LIBS_INIT})
add_definitions(-std=c++11)