cmake_minimum_required(VERSION 2.6)

project(idlc)

set(CMAKE_CXX_FLAG_DEBUG "${CMAKE_CXX_FLAG_DEBUG} -g -Wall -DDEBUG -D_DEBUG")
set(CMAKE_CXX_FLAG_RELEASE "${CMAKE_CXX_FLAG_RELEASE} -Wall")

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(GAMNET_VERSION 1.0.2)

add_definitions ( 
	-std=c++11
)

set(EXCLUDE_PATHS 
	"CMakeFiles" 
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

add_executable(idlc ${SRC}) 

