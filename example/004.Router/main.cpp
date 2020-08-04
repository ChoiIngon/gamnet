#include "UserSession.h"
#include <boost/program_options.hpp>
#include <boost/coroutine2/all.hpp>

#include <Gamnet/Library/MetaData.h>

void OnRouterConnect(const Gamnet::Network::Router::Address& address)
{
	LOG(INF, "OnConnect:", address.ToString());
}

void OnRouterClose(const Gamnet::Network::Router::Address& address)
{
	LOG(DEV, "OnClose:", address.ToString());
}


class MetaData : public Gamnet::MetaData
{
public :
	int			item_id;
	std::string	name;
	bool		Lock;
	float		attack;
	float		defense;
	int			price;
	Gamnet::Time::DateTime expire_date;

	MetaData()
		: GAMNET_INIT_MEMBER(item_id)
		, GAMNET_INIT_MEMBER(name)
		, GAMNET_INIT_MEMBER(Lock)
		, GAMNET_INIT_MEMBER(attack)
		, GAMNET_INIT_MEMBER(defense)
		, GAMNET_INIT_MEMBER(price)
		, GAMNET_INIT_MEMBER(expire_date)
	{
	}
};

int main(int argc, char** argv) 
{
	Gamnet::MetaDataReader<MetaData> reader;
	const std::list<std::shared_ptr<MetaData>>& metas = reader.Read("meta_data.csv");

	for(auto& meta : metas)
	{
		std::cout << meta->item_id << std::endl;
		std::cout << meta->expire_date << std::endl;
	}	
	
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

	Gamnet::InitCrashDump();
	try {
		Gamnet::Log::ReadXml(config_path);
		LOG(INF, argv[0], " Server Starts..");
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
		Gamnet::Network::Http::ReadXml(config_path);
		Gamnet::Network::Router::ReadXml(config_path, OnRouterConnect, OnRouterClose);
		Gamnet::Test::ReadXml<TestSession>(config_path);
		Gamnet::Run(vm["thread"].as<int>());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	return 0;
}


