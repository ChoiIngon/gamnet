cmake_minimum_required(VERSION 2.6)

project(Gamnet)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -DDEBUG -D_DEBUG -std=c++11")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -DDEBUG -D_DEBUG -std=c++11")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(GAMNET_VERSION 1.0.2)
configure_file(
	${PROJECT_SOURCE_DIR}/Gamnet.h.in
	${PROJECT_SOURCE_DIR}/Gamnet.h
)

if(UNIX AND NOT APPLE)
	set(LINUX true)
endif()

if(LINUX)
	include_directories(
	   	/usr/include/mysql
	)
endif()

if(APPLE)
	include_directories(
		/usr/local/Cellar/mysql-connector-c/6.1.6/include
		/usr/local/Cellar/boost/1.64.0_1/include
		/usr/local/Cellar/openssl/1.0.2l/include
		/usr/local/Cellar/curl/7.55.1/include
	)
endif()

#add_definitions ( 
#	-std=c++11
#)

set(EXCLUDE_PATHS 
	".settings" 
	"CMakeFiles" 
	"Debug" 
)

file(GLOB_RECURSE SRC "*.cpp" "*.c") 
foreach(TMP_PATH ${SRC})
	foreach(EXCLUDE_PATH ${EXCLUDE_PATHS})
		string(FIND ${TMP_PATH} ${EXCLUDE_PATH} RESULT)
		if(NOT ${RESULT} EQUAL -1)
			list(REMOVE_ITEM SRC ${TMP_PATH}) 
		endif()
	endforeach(EXCLUDE_PATH)
endforeach(TMP_PATH)

add_library(Gamnet STATIC ${SRC}) 

install (TARGETS Gamnet DESTINATION lib)
install (DIRECTORY ./ DESTINATION include/Gamnet 
	FILES_MATCHING PATTERN "*.h"
	PATTERN ".settings" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN "Debug" EXCLUDE
)
