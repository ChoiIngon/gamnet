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
public :
	class IColumn
	{
		const std::string name;
	public:
		IColumn(Meta* meta, const std::string& name);
		virtual void SetValue(const std::string& value) = 0;
	};

	template <class T>
	class Column : public IColumn
	{
	public :
		std::shared_ptr<T> value;
		Column(Meta* meta, const std::string& name) 
			: IColumn(meta, name)
			, value(std::make_shared<T>())
		{
		}

		virtual void SetValue(const std::string& value)
		{
			*this->value = boost::lexical_cast<T>(value);
		}

		operator T () const
		{
			return *value;
		}
	};

	class Text : public Column<std::string>
	{
	public :
		Text(Meta* meta, const std::string& name)
			: Column<std::string>(meta, name)
		{
		}
		operator const char* () const
		{
			return value->c_str();
		}
	};

	bool Init(const std::string fileName)
	{
		return true;
	}

	std::map<std::string, Meta::IColumn*>	meta_columns;
};

Meta::IColumn::IColumn(Meta* meta, const std::string& name)
		: name(name)
{
	meta->meta_columns.insert(std::make_pair(name, this));
}

class Something : public Meta
{
public :
	Meta::Column<std::string> column_1;

	Something()
		: column_1(this, "column_1")
	{
	}

	bool Init(const std::string& fileName)
	{
		meta_columns["column_1"]->SetValue("test");
		return true;
	}
};

template <class T>
std::ostream& operator << (std::ostream& os, const Meta::Column<T>& column)
{
	return os << *column.value;
}

bool operator == (const std::string& lhs, const Meta::Column<std::string>& rhs)
{
	return lhs == static_cast<std::string>(*rhs.value);
}

bool operator == (const Meta::Column<std::string>& lhs, const std::string& rhs)
{
	return rhs == lhs;
}

bool operator == (const std::string& lhs, const Meta::Column<std::string>& rhs)
{
	return lhs == static_cast<std::string>(*rhs.value);
}


int main(int argc, char** argv) 
{
	Something some;
	some.Init("");
	
	std::string test("test");
	if("test" == some.column_1)
	{
		std::cout << some.column_1 << "int" << std::endl;
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


