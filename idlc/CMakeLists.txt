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

add_definitions (
	-g -Wall -std=c++11 -DDEBUG -D_DEBUG
)

add_custom_command(OUTPUT "parser.h" "parser.cpp"
	COMMAND flex --nounistd --header-file=lexer.h -o lexer.cpp lexer.l
	COMMENT "flex --nounistd --header-file=lexer.h -o lexer.cpp lexer.l"
)

add_custom_command(OUTPUT "lexer.h" "lexer.cpp"
	COMMAND bison --defines=parser.h -o parser.cpp parser.y
	COMMENT "bison --defines=parser.h -o parser.cpp parser.y"
)

add_custom_target(pre_build
	COMMAND flex --nounistd --header-file=lexer.h -o lexer.cpp lexer.l
	COMMAND bison --defines=parser.h -o parser.cpp parser.y
	DEPENDS "lexer.h" "lexer.cpp" "parser.h" "parser.cpp"
)

set(EXCLUDE_PATHS 
	"CMakeFiles" 
	"data"
	"Debug"
	"x64"
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

add_dependencies(${CMAKE_PROJECT_NAME} pre_build)
