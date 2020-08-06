#
# https://gist.github.com/luncliff/6e2d4eb7ca29a0afd5b592f72b80cb5c
#

cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_NAME ${CMAKE_SOURCE_DIR} NAME)
string(REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME})

project(${PROJECT_NAME})

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

if(UNIX AND NOT APPLE) 
	set(LINUX true) 
endif()

if(LINUX) 
	include_directories( 
		../../
		/usr/include/mysql 
	) 
	link_directories(
		../../Gamnet/Debug
		/usr/lib/x86_64-linux-gnu
	) 
endif()

if(APPLE) 
	include_directories( 
		../../
		/usr/local/include/Gamnet 
		/usr/local/Cellar/mysql-connector-c/6.1.6/include 
		/usr/local/Cellar/boost/1.64.0_1/include
	       	/usr/local/Cellar/openssl/1.0.2l/include
	       	/usr/local/Cellar/curl/7.55.1/include
	)
	link_directories(
		../../Gamnet/Debug
	       	/usr/local/lib
	       	/usr/local/Cellar/boost/1.64.0_1/lib
	       	/usr/local/Cellar/curl/7.55.1/lib
	       	/usr/local/Cellar/mysql-connector-c/6.1.6/lib
	       	/usr/local/Cellar/openssl/1.0.2l/lib
	)
endif() 
 
link_libraries( 
	Gamnet
	curl
	boost_filesystem
	boost_system
	boost_program_options
	boost_date_time
	pthread
)

add_definitions (
	-g -Wall -std=c++11 -DDEBUG -D_DEBUG
)

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

add_executable(${CMAKE_PROJECT_NAME} ${SRC})
add_subdirectory(../../Gamnet ../../Gamnet/Debug)
