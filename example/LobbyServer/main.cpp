#include "UserSession.h"
#include <boost/program_options.hpp>
#include "Component/Dungeon/Unit/Monster.h"

int main(int argc, char** argv) 
{
	/*
	BehaviourTree::Meta meta;
	meta.BindExcutorCreator<RiseFromChair>("RiseFromChair");
	meta.BindExcutorCreator<MoveToVendingMachine>("MoveToVendingMachine");
	meta.BindExcutorCreator<BuyTea>("BuyTea");
	meta.BindExcutorCreator<BuyCoffee>("BuyCoffee");
	meta.BindExcutorCreator<ReturnToChair>("ReturnToChair");
	meta.ReadXml("../MetaData/BehaviourTree.xml");
	std::shared_ptr<BehaviourTree> behaviour = meta.Create();

	std::shared_ptr<Gamnet::Component> component = std::make_shared<Gamnet::Component>();
	std::shared_ptr<Player> player = component->AddComponent<Player>();
	player->money = 0;
	behaviour->Run(component);

	player->money = 200;
	behaviour->Run(component);
	*/
	boost::program_options::options_description desc("All Options");
	desc.add_options()
		("config", boost::program_options::value<std::string>()->default_value("config.xml"), "config file path")
		("thread", boost::program_options::value<int>()->default_value(std::thread::hardware_concurrency()), "working thread count")
		("help", "product help message");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if(0 != vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 1;
	}
	const char* config_path = vm["config"].as<std::string>().c_str();

	Gamnet::InitCrashDump();
	try {
		Gamnet::Log::ReadXml(config_path);
		LOG(INF, argv[0], " Server Starts.. config=", config_path);
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Database::MySQL::ReadXml(config_path);
		//Gamnet::Database::Redis::ReadXml(config_path);
		//Gamnet::Database::SQLite::ReadXml(config_path);

		Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
		Gamnet::Network::Http::ReadXml(config_path);
		Gamnet::Test::ReadXml<TestSession>(config_path);
		Gamnet::Run(vm["thread"].as<int>());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		return 1;
	}
	return 0;
}


