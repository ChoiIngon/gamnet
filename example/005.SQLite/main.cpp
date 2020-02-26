#include <Gamnet.h>
#include <boost/program_options.hpp>

enum SQLITE_DB
{
	INVALID = 0,
	USER_DATA = 1
};

int main(int argc, char** argv) 
{
	boost::program_options::options_description desc("All Options");
	desc.add_options()
		("config", boost::program_options::value<std::string>()->default_value("config.xml"), "config file path")
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
	Gamnet::Log::ReadXml(config_path);
	LOG(INF, "005.SQLite Server Starts..with config=", config_path);
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Database::SQLite::Connect(SQLITE_DB::USER_DATA, "user_db");
		Gamnet::Database::SQLite::Execute(SQLITE_DB::USER_DATA, "DROP TABLE IF EXISTS user_data");
		Gamnet::Database::SQLite::Execute(SQLITE_DB::USER_DATA, 
			"CREATE TABLE `user_data` ("
				"`user_id` TEXT NOT NULL PRIMARY KEY,"
				"`user_seq` INTEGER NOT NULL"
			")"
		);

		Gamnet::Database::SQLite::Execute(SQLITE_DB::USER_DATA,
			"INSERT INTO `user_data`(`user_id`, `user_seq`) VALUES('USER_001', 1)"
		);
		Gamnet::Database::SQLite::Execute(SQLITE_DB::USER_DATA, 
			"INSERT INTO `user_data`(`user_id`, `user_seq`) VALUES('USER_002', 2)"
		);

		Gamnet::Database::SQLite::ResultSet rows = Gamnet::Database::SQLite::Execute(SQLITE_DB::USER_DATA, "SELECT `user_id`, `user_seq` FROM `user_data`");
		for(const auto& row : rows)
		{
			LOG(INF, "user_id:", (std::string)row["user_id"], ", user_seq:", (float)row["user_seq"]);
		}
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	std::cin.get();
	return 0;
}


