#include "ChatSession.h"
#include <Network/Acceptor.h>

int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "003.Chat Server Starts..");
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Network::Tcp::ReadXml<ChatSession>("config.xml");
		//Gamnet::Network::Router::ReadXml("config.xml", [](const Gamnet::Network::Router::Address&) {}, [](const Gamnet::Network::Router::Address&) {});
		//Gamnet::Network::Http::Listen(30001);

		Gamnet::Test::ReadXml<TestSession>("config.xml");
		Gamnet::Run(30 /*std::thread::hardware_concurrency()*/);
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	return 0;
}


