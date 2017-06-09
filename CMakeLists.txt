cmake_minimum_required(VERSION 3.4)
project(HeckaDBMS)

set(CMAKE_CXX_STANDARD 11)

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()
set(BOOST_ROOT "/usr/local/boost_1_61_0/")

set(DIR_ROOT "${CMAKE_SOURCE_DIR}")
set(DIR_CLIENT "client")
set(DIR_DM "dm")
set(DIR_TM "tm")
set(DIR_TWOPL "tm/twopl")
set(DIR_HEKATON "tm/hekaton")

set(CMAKE_C_FLAGS "-pthread -g -Wall")

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
    ${DIR_DM}/Record.cpp ${DIR_DM}/Record.h
    ${DIR_DM}/Timestamp.cpp ${DIR_DM}/Timestamp.h
    ${DIR_TM}/TransactionManager.cpp ${DIR_TM}/TransactionManager.h
    ${DIR_TM}/Operation.cpp ${DIR_TM}/Operation.h
    ${DIR_HEKATON}/Hekaton.cpp ${DIR_HEKATON}/Hekaton.h
    ${DIR_HEKATON}/Transaction.cpp ${DIR_HEKATON}/Transaction.h
    ${DIR_TWOPL}/TwoPL.cpp ${DIR_TWOPL}/TwoPL.h
    ${DIR_TWOPL}/LockManager.cpp ${DIR_TWOPL}/LockManager.h
    ${DIR_TWOPL}/LockEntry.cpp ${DIR_TWOPL}/LockEntry.h
    ${DIR_TWOPL}/Lock.cpp ${DIR_TWOPL}/Lock.h)
  
add_executable(HeckaDBMS ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(HeckaDBMS pthread)

