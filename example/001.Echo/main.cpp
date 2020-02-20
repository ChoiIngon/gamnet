#include "EchoSession.h"

int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "001.Echo Example Server Starts..");
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Network::Tcp::ReadXml<EchoSession>("config.xml");
		Gamnet::Network::Router::ReadXml("config.xml");
		Gamnet::Network::Http::Listen(10002);
		
		Gamnet::Test::ReadXml<TestSession>("config.xml");
		Gamnet::Run(std::thread::hardware_concurrency());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	return 0;
}


