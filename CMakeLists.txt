cmake_minimum_required(VERSION 3.4)
project(HeckaDBMS)

set(CMAKE_CXX_STANDARD 11)

SET(DIR_ROOT "${CMAKE_SOURCE_DIR}")
set(DIR_CLIENT "client")
set(DIR_DM "dm")
set(DIR_TM "tm")
set(DIR_TWOPL "tm/twopl")
set(DIR_HEKATON "tm/hekaton")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -Wall -pthread")

INCLUDE_DIRECTORIES(
    ${DIR_ROOT}
    ${DIR_CLIENT}
    ${DIR_TM}
    ${DIR_TWOPL}
    ${DIR_HEKATON}
    ${DIR_DM})

set(SOURCE_FILES
    Main.cpp
    Utility.cpp Utility.h
    ${DIR_CLIENT}/ManualTest.cpp ${DIR_CLIENT}/ManualTest.h
    ${DIR_CLIENT}/ScaleTest.cpp ${DIR_CLIENT}/ScaleTest.h
    ${DIR_CLIENT}/VaryTest.cpp ${DIR_CLIENT}/VaryTest.h
    ${DIR_CLIENT}/Client.cpp ${DIR_CLIENT}/Client.h
    ${DIR_DM}/DataManager.cpp ${DIR_DM}/DataManager.h
    ${DIR_TM}/TransactionManger.cpp ${DIR_TM}/TransactionManager.h
    ${DIR_HEKATON}/Record.cpp ${DIR_HEKATON}/Record.h
    ${DIR_HEKATON}/Timestamp.cpp ${DIR_HEKATON}/Timestamp.h
    ${DIR_HEKATON}/Transaction.cpp ${DIR_HEKATON}/Transaction.h
    ${DIR_HEKATON}/Hekaton.cpp ${DIR_HEKATON}/Hekaton.h
    ${DIR_TWOPL}/TwoPL.cpp ${DIR_TWOPL}/TwoPL.h)

add_executable(HeckaDBMS ${SOURCE_FILES})
