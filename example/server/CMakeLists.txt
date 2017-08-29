project(server)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(
   	/usr/include/mysql
)

link_directories(
	/usr/lib/x86_64-linux-gnu
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

