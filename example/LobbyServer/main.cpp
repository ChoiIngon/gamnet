#include "UserSession.h"
#include <boost/program_options.hpp>
#include "Component/Dungeon/Unit/BehaviourTree.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>


class Player
{
public :
	int money;
};

class PlayerBehaviour : public BehaviourTree
{
public:
	std::shared_ptr<Player> player;
};

class Executor : public BehaviourTree::Node
{
public :
	Executor(const std::shared_ptr<PlayerBehaviour>& tree, const std::string& name)
		: tree(tree), name(name)
	{
	}

	const std::string name;
protected :
	std::weak_ptr<PlayerBehaviour> tree;
};

class ExecutorFactory
{
public :
	std::shared_ptr<Executor> Create(std::shared_ptr<PlayerBehaviour>& tree, const std::string& name)
	{
		return creators[name](tree);
	}

	template<class T>
	void BindExcutorCreate(const std::string& name)
	{
		creators[name] = [](std::shared_ptr<PlayerBehaviour>& tree)
		{
			return std::make_shared<T>(tree);
		};
	}
private :
	std::map<std::string, std::function<std::shared_ptr<Executor>(std::shared_ptr<PlayerBehaviour>&)>> creators;
};

ExecutorFactory executor_factory;

class RiseFromChair : public Executor
{
public :
	RiseFromChair(const std::shared_ptr<PlayerBehaviour>& tree)
		: Executor(tree, "RiseFromChair")
	{
	}

	bool Run() override
	{
		std::cout << "rise from chair" << std::endl;
		return true;
	}
};

class MoveToVendingMachine : public Executor
{
public:
	MoveToVendingMachine(const std::shared_ptr<PlayerBehaviour>& tree)
		: Executor(tree, "MoveToVendingMachine")
	{
	}

	bool Run() override
	{
		std::shared_ptr<Player> player = tree.lock()->player;
		if(200 <= player->money)
		{
			std::cout << "move to vending machine" << std::endl;
			return true;
		}
		return false;
	}
};

class BuyTea : public Executor
{
public:
	BuyTea(const std::shared_ptr<PlayerBehaviour>& tree)
		: Executor(tree, "BuyTea")
	{
	}

	bool Run() override
	{
		std::cout << "buy a cup of tea" << std::endl;
		return true;
	}
};

class BuyCoffee : public Executor
{
public:
	BuyCoffee(const std::shared_ptr<PlayerBehaviour>& tree)
		: Executor(tree, "BuyTea")
	{
	}

	bool Run() override
	{
		std::cout << "buy a cup of coffee" << std::endl;
		return true;
	}
};

class ReturnToChair : public Executor
{
public:
	ReturnToChair(const std::shared_ptr<PlayerBehaviour>& tree)
		: Executor(tree, "ReturnToChair")
	{
	}

	bool Run() override
	{
		std::cout << "return to chair" << std::endl;
		return true;
	}
};

void LoadBehaviour(std::shared_ptr<PlayerBehaviour> behaviour, const std::shared_ptr<BehaviourTree::Node>& parent, boost::property_tree::ptree& ptree)
{
	for (auto& child : ptree.get_child(""))
	{
		if ("<xmlattr>" == child.first)
		{
			return;
		}

		std::shared_ptr<BehaviourTree::Node> node = nullptr;
		
		if("Sequence" == child.first)
		{
			node = std::make_shared< BehaviourTree::Sequence>();
		}
		if("RandomSelector" == child.first)
		{
			node = std::make_shared< BehaviourTree::RandomSelector>();
		}
		else if("Executor" == child.first)
		{
			const std::string& className = child.second.get_child("<xmlattr>.class").get_value<std::string>();
			node = executor_factory.Create(behaviour, className);
		}
				
		parent->AddChild(node);
		LoadBehaviour(behaviour, node, child.second);
		//std::cout << "element:" << indent << child.first << std::endl;;
	}
}

std::shared_ptr<PlayerBehaviour> LoadBehaviour(const std::string& path)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		throw Gamnet::Exception(Gamnet::ErrorCode::FileNotFound, e.what());
	}

	std::shared_ptr<PlayerBehaviour> behaviour = std::make_shared<PlayerBehaviour>();
	try {
		for(auto& child : ptree_.get_child(""))
		{
			LoadBehaviour(behaviour, behaviour->root, child.second);
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		throw GAMNET_EXCEPTION(Gamnet::ErrorCode::SystemInitializeError, e.what());
	}
	return behaviour;
}

int main(int argc, char** argv) 
{
	executor_factory.BindExcutorCreate<RiseFromChair>("RiseFromChair");
	executor_factory.BindExcutorCreate<MoveToVendingMachine>("MoveToVendingMachine");
	executor_factory.BindExcutorCreate<BuyTea>("BuyTea");
	executor_factory.BindExcutorCreate<BuyCoffee>("BuyCoffee");
	executor_factory.BindExcutorCreate<ReturnToChair>("ReturnToChair");

	std::shared_ptr<PlayerBehaviour> behaviour = LoadBehaviour("../MetaData/BehaviourTree.xml");

	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->money = 0;
	behaviour->player = player;

	behaviour->Run();
	player->money = 200;
	for(int i=0; i<10; i++)
	{
		behaviour->Run();
	}
	
	// for console command arguments
	// <Binary>.exe --config=<config file path>
	// eg) Windows : 005.Database.exe --config=config.xml
	// eg) Linux : ./005.Database --config=config.xml
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


