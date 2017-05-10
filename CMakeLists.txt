cmake_minimum_required(VERSION 3.7)
project(HeckaDBMS)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp db/DBMS.cpp db/DBMS.h)
add_executable(HeckaDBMS ${SOURCE_FILES})