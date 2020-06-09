#include "UserSession.h"
#include "Handler_SendMessage.h"
#include <boost/program_options.hpp>

void OnRouterConnect(const Gamnet::Network::Router::Address& address)
{
	LOG(INF, "OnConnect:", address.ToString());
}

void OnRouterClose(const Gamnet::Network::Router::Address& address)
{
	LOG(DEV, "OnClose:", address.ToString());
}

static boost::asio::io_service& io_service_ = Gamnet::Singleton<boost::asio::io_service>::GetInstance();

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

	const char* config_path = vm["config"].as<std::string>().c_str();

	try {
		Gamnet::Log::ReadXml(config_path);
		Gamnet::Singleton<Manager_Session>::GetInstance().Init();
		LOG(INF, argv[0], " Server Starts..");
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
		Gamnet::Network::Http::ReadXml(config_path);
		
		Gamnet::Network::Router::ReadXml(config_path, OnRouterConnect, OnRouterClose);
		Gamnet::Test::ReadXml<TestSession>(config_path);
		//Gamnet::Run(vm["thread"].as<int>());
		Gamnet::Singleton<boost::asio::io_service>::GetInstance().run();
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	return 0;
}


