cmake_minimum_required(VERSION 3.4)
project(HeckaDBMS)

set(CMAKE_CXX_STANDARD 11)

SET(DIR_ROOT "${CMAKE_SOURCE_DIR}")
set(DIR_TM "tm")
set(DIR_2PL "tm/2pl")
set(DIR_HEKATON "tm/hekaton")
set(DIR_DM "dm")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")

INCLUDE_DIRECTORIES(
        ${DIR_ROOT}
        ${DIR_TM}
        ${DIR_2PL}
        ${DIR_HEKATON}
        ${DIR_DM})

set(SOURCE_FILES
        Main.cpp
        ${DIR_DM}/DataManager.cpp ${DIR_DM}/DataManager.h
        ${DIR_HEKATON}/Record.cpp ${DIR_HEKATON}/Record.h
        ${DIR_HEKATON}/Timestamp.cpp ${DIR_HEKATON}/Timestamp.h client/Manual.cpp client/Manual.h client/Scale.cpp client/Scale.h client/Vary.cpp client/Vary.h client/Client.cpp client/Client.h Utility.cpp Utility.h)

add_executable(HeckaDBMS ${SOURCE_FILES})