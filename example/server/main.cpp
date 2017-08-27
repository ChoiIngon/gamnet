#include "Session.h"

int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "Server Starts..");
	LOG(INF, "boost version:", BOOST_LIB_VERSION);
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Network::Tcp::Listen<Session>(20000, 1024, 300);
		Gamnet::Network::Router::Listen("GAME", 20001, 
			[](const Gamnet::Network::Router::Address& addr) {
				LOG(DEV, "Router::OnAccept(address:", addr.service_name, ":", (int)addr.cast_type, ":", addr.id, ")");
			}, 
			[](const Gamnet::Network::Router::Address& addr) {
				LOG(DEV, "Router::OnClose(address:", addr.service_name, ":", (int)addr.cast_type, ":", addr.id, ")");
			}
		);
	
		Gamnet::Network::Router::Connect("127.0.0.1", 20001, 300, 
			[](const Gamnet::Network::Router::Address& addr) {
				LOG(DEV, "Router::OnConnect(address:", addr.service_name, ":", (int)addr.cast_type, ":", addr.id, ")");
			},
			[](const Gamnet::Network::Router::Address& addr) {
				LOG(DEV, "Router::OnClose(address:", addr.service_name, ":", (int)addr.cast_type, ":", addr.id, ")");
			}
		);
	
		Gamnet::Network::Http::Listen(8080, 1024, 300);

		Gamnet::Test::ReadXml<TestSession>("config.xml");
		Gamnet::Run(std::thread::hardware_concurrency());
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(ERR, "exception(error_code:", e.error_code(), ", message:", e.what(), ")");
	}
	return 0;
}


