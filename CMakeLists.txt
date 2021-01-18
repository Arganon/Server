cmake_minimum_required (VERSION 3.5)

project (server LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -pedantic -lssl -lcrypto")

set (SOURCE utils/src/paramparser.cpp
            utils/src/requestparser.cpp
	    server.cpp
	    common/src/requesthandler.cpp
	    common/src/storage.cpp)

set (HEADERS utils/public/paramparser.h
             utils/public/requestparser.h
	     utils/public/logger.h
	     data/public/types.h
	     data/public/session.h
	     data/public/httprequest.h
	     data/public/httpresponse.h
	     data/public/data_structures.h
	     server.h
	     common/public/requesthandler.h
	     common/public/storage.h)

set (SOURCE_FILES main.cpp ${SOURCE} ${HEADERS})

add_executable (server ${SOURCE_FILES})

TARGET_LINK_LIBRARIES (server pthread OpenSSL::SSL OpenSSL::Crypto)
