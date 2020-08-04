#include "UserSession.h"
#include <boost/program_options.hpp>
#include <boost/coroutine2/all.hpp>

void OnRouterConnect(const Gamnet::Network::Router::Address& address)
{
	LOG(INF, "OnConnect:", address.ToString());
}

void OnRouterClose(const Gamnet::Network::Router::Address& address)
{
	LOG(DEV, "OnClose:", address.ToString());
}

class Meta 
{
	std::map<std::string, std::function<void(const std::string&)>>	bind_functions;
protected :

	std::string Bind(const std::string& name, std::string& member)
	{
		bind_functions.insert(std::make_pair(name, [&member](const std::string& value) {
			member = value;
		}));
		return "";
	}

	int Bind(const std::string& name, int& member)
	{
		bind_functions.insert(std::make_pair(name, [&member](const std::string& value) {
			member = boost::lexical_cast<int>(value);
		}));
		return 0;
	}

	Gamnet::Time::DateTime Bind(const std::string& name, Gamnet::Time::DateTime& member)
	{
		bind_functions.insert(std::make_pair(name, [&member](const std::string& value) {
			member = value;
		}));
		return Gamnet::Time::DateTime::MinValue;
	}

	float Bind(const std::string& name, float& member)
	{
		bind_functions.insert(std::make_pair(name, [&member](const std::string& value) {
			member = boost::lexical_cast<float>(value);
		}));
		return 0.0f;
	}

	template <class T>
	T Bind(const std::string& name, T& member)
	{
		return T();
	}

	void SetValue(const std::string& name, const std::string& value)
	{
		bind_functions[name](value);
	}
public :
	bool Init(const Json::Value& row)
	{
		for (auto& itr : bind_functions)
		{
			SetValue(itr.first, row[itr.first].asString());
		}
		return true;
	}
};

class Something : public Meta
{
public :
	std::string text;
	int number;
	Gamnet::Time::DateTime date;
	float point;
	Something()
		: text(Bind("text", text))
		, number(Bind("number", number))
		, date(Bind("date", date))
		, point(Bind("point", point))
	{
	}

	
};

int main(int argc, char** argv) 
{
	Something some;

	Json::Value root;
	root["text"] = "text";
	root["number"] = "10";
	root["date"] = "2018-01-01 00:00:00";
	root["point"] = "100.0";

	some.Init(root);
	
	std::string test("test");
	if("text" == some.text)
	{
		std::cout << some.text << "int" << std::endl;
	}

	std::cout << some.date.ToString() << std::endl;
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


