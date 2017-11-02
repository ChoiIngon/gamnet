cmake_minimum_required(VERSION 2.6)

project(server)
	
set(GAMNET_VERSION 1.0.1)
configure_file(
	${PROJECT_SOURCE_DIR}/Gamnet.h.in
	${PROJECT_SOURCE_DIR}/Gamnet.h
)

set(CMAKE_CXX_FLAG_DEBUG "${CMAKE_CXX_FLAG_DEBUG} -g -Wall -DDEBUG -D_DEBUG")
set(CMAKE_CXX_FLAG_RELEASE "${CMAKE_CXX_FLAG_RELEASE} -Wall")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(
   	/usr/include/mysql
)

#include_directories(
#   	/usr/local/Cellar/curl/7.55.1/include
#	/usr/local/Cellar/mysql-connector-c/6.1.6/include
#   	/usr/local/Cellar/openssl/1.0.2l/include
#)

link_directories(
	/usr/lib/x86_64-linux-gnu
)

add_definitions ( 
	-std=c++11
)

add_library(Gamnet STATIC
	Database/MySQL/Connection.cpp
	Database/MySQL/MySQL.cpp
	Database/MySQL/ResultSet.cpp
	Database/MySQL/Transaction.cpp
	Database/Redis/Connection.cpp
	Database/Redis/Redis.cpp
	Database/Redis/ResultSet.cpp
	Database/Redis/Transaction.cpp
	Database/Database.cpp
	Library/Json/jsoncpp.cpp
	Library/Base64.cpp
	Library/Buffer.cpp
	Library/Exception.cpp
	Library/MD5.cpp
	Library/Random.cpp
	Library/Variant.cpp
	Log/File.cpp
	Log/Log.cpp
	Log/Logger.cpp
	Network/HandlerContainer.cpp
	Network/Link.cpp
	Network/LinkManager.cpp
	Network/Session.cpp
	Network/Http/
	Network/Http/Dispatcher.cpp
	Network/Http/HttpClient.cpp
	Network/Http/HttpServer.cpp
	Network/Http/LinkManager.cpp
	Network/Http/Request.cpp
	Network/Http/Response.cpp
	Network/Http/Session.cpp
	Network/Router/LinkManager.cpp
	Network/Router/Router.cpp
	Network/Router/RouterCaster.cpp
	Network/Router/RouterHandler.cpp
	Network/Router/Session.cpp
	Network/Tcp/Packet.cpp
	Network/Tcp/Session.cpp
	Network/Tcp/Tcp.cpp
	Test/Session.cpp
	Gamnet.cpp
)

install (TARGETS Gamnet DESTINATION lib)
install (DIRECTORY ./ DESTINATION include/Gamnet 
	FILES_MATCHING PATTERN "*.h"
	PATTERN "Debug" EXCLUDE
	PATTERN "CMakeFiles" EXCLUDE
	PATTERN ".settings" EXCLUDE
)
