cmake_minimum_required(VERSION 3.1)

set(SOURCE_FILES main.cpp)
add_executable(mydbms ${SOURCE_FILES} externalsort.h externalsort.cpp)