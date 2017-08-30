project(server)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#include_directories(
#   	/usr/include/mysql
#)

include_directories(
   	#/usr/local/Cellar/boost/1.64.0_1/include
   	/usr/local/Cellar/curl/7.55.1/include
	/usr/local/Cellar/mysql-connector-c/6.1.6/include
   	/usr/local/Cellar/openssl/1.0.2l/include
)

#link_directories(
#	/usr/lib/x86_64-linux-gnu
#)

link_directories(
	/usr/local/lib
	/usr/local/Cellar/boost/1.64.0_1/lib
	/usr/local/Cellar/curl/7.55.1/lib
	/usr/local/Cellar/mysql-connector-c/6.1.6/lib
	/usr/local/Cellar/openssl/1.0.2l/lib
)

link_libraries(
	Gamnet
	curl
	boost_filesystem
	boost_system
	pthread
)

add_definitions (
	-g -Wall -std=c++11 -DDEBUG -D_DEBUG
)

add_executable(server
	Handler_HeartBeat.cpp
	Handler_Http_ServerState.cpp
	Handler_Login.cpp
	Manager_Session.cpp
	Session.cpp
	main.cpp
)

