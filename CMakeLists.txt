cmake_minimum_required(VERSION 3.5)

project(server LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -pedantic")

set (SOURCE paramparser.cpp
            server.cpp
            threadpool.cpp)

set (HEADERS paramparser.h
             logger.h
             server.h
             threadpool.h
             types.h)

set (SOURCE_FILES main.cpp ${SOURCE} ${HEADERS})

add_executable (server ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(server pthread)
