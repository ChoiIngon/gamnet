#include <boost/program_options.hpp>
#include <Network/Http/HttpServer.h>
#include <Test/SessionManager.h>
#include "UserSession.h"

int main(int argc, char** argv) 
{
	boost::program_options::options_description desc("All Options");
	desc.add_options()
		("config", boost::program_options::value<std::string>()->default_value("config.xml"), "config file path")
		("thread", boost::program_options::value<int>()->default_value(std::thread::hardware_concurrency()), "working thread count")
		("help", "");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (0 != vm.count("help"))
	{
		std::cerr << desc << std::endl;
		return 1;
	}

	const std::string& config_path = vm["config"].as<std::string>();
	Gamnet::Log::ReadXml(config_path);
	LOG(INF, argv[0], " Server Starts..");
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
	Gamnet::Network::Http::ReadXml(config_path);
	Gamnet::Test::ReadXml<TestSession>(config_path);
	Gamnet::Singleton<boost::asio::io_service>::GetInstance().run();
	return 0;
}


