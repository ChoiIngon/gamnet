#include "Session.h"

int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "Server Starts..");
	LOG(INF, "boost version:", BOOST_LIB_VERSION);
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	Gamnet::Network::Tcp::Listen<Session>(20000, 1024, 300);
	Gamnet::Test::ReadXml<TestSession>("config.xml");

	Gamnet::Run(std::thread::hardware_concurrency());
	return 0;
}


