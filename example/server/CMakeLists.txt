project(server)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(
	/usr/local/include
   	/usr/include/mysql
)

link_directories(
	../../Gamnet/Debug	
)

link_libraries(
	/usr/local/lib
	Gamnet
	curl
	boost_filesystem
	boost_system
	pthread
)

add_definitions (
	-g -Wall -std=c++11
)

add_executable(server
	Handler_HeartBeat.cpp
	Handler_Http_ServerState.cpp
	Handler_Login.cpp
	Manager_Session.cpp
	Session.cpp
	main.cpp
)

