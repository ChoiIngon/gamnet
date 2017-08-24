project(server)

include_directories(
	/usr/local/include
   	/usr/include/mysql
)

add_definitions (
	-g -Wall -std=c++11
)

add_library(Gamnet STATIC
	Database/MySQL/Connection.cpp
	Database/MySQL/MySQL.cpp
	Database/MySQL/ResultSet.cpp
	Database/MySQL/Transaction.cpp
	Database/Redis/Connection.cpp
	Database/Redis/Redis.cpp
	Database/Redis/ResultSet.cpp
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

