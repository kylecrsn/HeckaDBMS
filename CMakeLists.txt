cmake_minimum_required(VERSION 3.4)
project(HeckaDBMS)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp db/DBMS.cpp db/DBMS.h hekaton/Record.cpp hekaton/Record.h hekaton/Timestamp.cpp hekaton/Timestamp.h)
add_executable(HeckaDBMS ${SOURCE_FILES})