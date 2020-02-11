#include "ChatSession.h"

int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "Server Starts..");
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Network::Tcp::ReadXml<ChatSession>("config.xml");
		Gamnet::Network::Http::Listen(20001);
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


