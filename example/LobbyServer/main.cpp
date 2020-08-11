#include "UserSession.h"
#include <boost/program_options.hpp>

#include <Gamnet/Library/MetaData.h>
enum DATABASE_TYPE
{
	INVALID = 0,
	USER_DATA = 1,
	CONFIG_DB_ID = 2
};

void Example_MySQL(const char* config_path)
{
	using namespace Gamnet::Database::MySQL;

	// 0. prepare database and user account
	//		mysql> use mysql;
	//		mysql> CREATE USER 'gamnet'@'%' identified by 'P@ssword!';
	//		mysql> GRANT ALL PRIVILEGES ON *.* to 'gamnet'@'%';
	//		mysql> CREATE USER 'gamnet'@'localhost' identified by 'P@ssword!';
	//		mysql> GRANT ALL PRIVILEGES ON *.* to 'gamnet'@'localhost';
	//		mysql> FLUSH PRIVILEGES;
	//		mysql> CREATE DATABASE user_data;
	
	// 1. connect to mysql server
	//  db_type :			
	//		you can access this connection by index(DATABASE_TYPE::USER_DATA = 1).
	//		if connection information(host, port..etc) is changed, the only thing you have to do is chage parameter of 'Connect' function
	//	fail_query_log : 
	//		When server can't execute queries(database isn't working properly, lost connection or for any reasons). 
	//		if fail_query_log is true, Gamnet write failed query itself to log files.
	Connect(DATABASE_TYPE::USER_DATA, "localhost", 3306, "gamnet", "P@ssword!", "user_data", true);

	// 2. create tables for test
	Execute(DATABASE_TYPE::USER_DATA, 
		"DROP TABLE IF EXISTS user_data"
	);
	Execute(DATABASE_TYPE::USER_DATA, 
		"CREATE TABLE `user_data` ("
		"`user_seq` INT NOT NULL AUTO_INCREMENT,"
		"`user_id` VARCHAR(20) NOT NULL DEFAULT '',"
		"PRIMARY KEY(`user_seq`),"
		"UNIQUE INDEX `UNIQUE_IDX` (`user_id`)"
		")"
	);
	Execute(DATABASE_TYPE::USER_DATA, 
		"DROP TABLE IF EXISTS item_data"
	);
	Execute(DATABASE_TYPE::USER_DATA, 
		"CREATE TABLE `item_data` ("
		"`item_seq` INT NOT NULL AUTO_INCREMENT,"
		"`user_seq` INT NOT NULL,"
		"`item_id` VARCHAR(20) NOT NULL DEFAULT '',"
		"PRIMARY KEY(`item_seq`)"
		")"
	);

	// 3. if you need guarantee between each query. use 'Transaction'.
	// all Transaction should be finished with 'Commit'
	for(int i=0; i<100; i++)
	{
		Transaction transaction(DATABASE_TYPE::USER_DATA);
		transaction.Execute("INSERT INTO user_data(user_seq, user_id) values(", i + 1, ",'USER_", i + 1, "')");
		transaction.Execute("INSERT INTO item_data(user_seq, item_id) values(", i + 1, ",'ITEM_", i + 1, "')");
		transaction.Commit();
	}
	{
		ResultSet ret = Execute(DATABASE_TYPE::USER_DATA,
			"SELECT item_seq, user_seq, item_id from item_data"
		);
		for(const auto& row : ret)
		{
			LOG(INF, "[MySQL] item_seq:", (int)row["item_seq"], ", user_seq:", (int)row["user_seq"], ", item_id:", (std::string)row["item_id"]);
		}
	}

	// using config file
	ReadXml(config_path);
	{
		ResultSet ret = Execute(DATABASE_TYPE::CONFIG_DB_ID,
			"SELECT item_seq, user_seq, item_id from item_data"
		);
		for (const auto& row : ret)
		{
			LOG(INF, "[MySQL] item_seq:", (int)row["item_seq"], ", user_seq:", (int)row["user_seq"], ", item_id:", (std::string)row["item_id"]);
		}
	}
}

void Example_SQLite(const char* config_path)
{
	using namespace Gamnet::Database::SQLite;
	// 1. create sqlite database file
	//		In this case. you can access "user_data database" by index(DATABASE_TYPE::USER_DATA = 1)
	Connect(DATABASE_TYPE::USER_DATA, "user_data.sqlite");

	// 2. create table.
	Execute(DATABASE_TYPE::USER_DATA, 
		"DROP TABLE IF EXISTS user_data"
	);
	Execute(DATABASE_TYPE::USER_DATA,
		"CREATE TABLE `user_data` ("
		"`user_seq`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
		"`user_id`	TEXT NOT NULL DEFAULT ''"
		")"
	);
	Execute(DATABASE_TYPE::USER_DATA, 
		"CREATE UNIQUE INDEX `USER_DATA_UNIQUE_IDX` ON `user_data` ("
		"`user_id`"
		")"
	);
	Execute(DATABASE_TYPE::USER_DATA,
		"DROP TABLE IF EXISTS item_data"
	);
	Execute(DATABASE_TYPE::USER_DATA,
		"CREATE TABLE `item_data` ("
		"`item_seq` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
		"`user_seq` INTEGER NOT NULL,"
		"`item_id` TEXT NOT NULL DEFAULT ''"
		")"
	);

	// 3. if you need guarantee between each query. use 'Transaction'.
	// all Transaction should be finished with 'Commit'
	for (int i = 0; i<100; i++)
	{
		Transaction transaction(DATABASE_TYPE::USER_DATA);
		transaction.Execute("INSERT INTO user_data(user_seq, user_id) values(", i + 1, ",'USER_", i + 1, "')");
		transaction.Execute("INSERT INTO item_data(user_seq, item_id) values(", i + 1, ",'ITEM_", i + 1, "')");
		transaction.Commit();
	}

	{
		ResultSet ret = Execute(DATABASE_TYPE::USER_DATA,
			"SELECT item_seq, user_seq, item_id from item_data"
		);
		for (const auto& row : ret)
		{
			LOG(INF, "[SQLite] item_seq:", (int)row["item_seq"], ", user_seq:", (int)row["user_seq"], ", item_id:", (std::string)row["item_id"]);
		}
	}

	// using config file
	ReadXml(config_path);
	{
		ResultSet ret = Execute(DATABASE_TYPE::CONFIG_DB_ID,
			"SELECT item_seq, user_seq, item_id from item_data"
		);
		for (const auto& row : ret)
		{
			LOG(INF, "[MySQL] item_seq:", (int)row["item_seq"], ", user_seq:", (int)row["user_seq"], ", item_id:", (std::string)row["item_id"]);
		}
	}
}

void Example_Redis(const char* config_path)
{
	using namespace Gamnet::Database::Redis;

	// 1. connect to redis server
	//  db_type :			
	//		you can access this connection by index(DATABASE_TYPE::USER_DATA = 1).
	//		if connection information(host, port..etc) is changed, the only thing you have to do is chage parameter of 'Connect' function
	Connect(DATABASE_TYPE::USER_DATA, "localhost", 6379);
	Execute(DATABASE_TYPE::USER_DATA, "FLUSHALL");
	for(int i=0; i<100; i++)
	{
		Set(DATABASE_TYPE::USER_DATA, Gamnet::Format(i + 1), Gamnet::Format("USER_", i + 1));
		std::string value = Get(DATABASE_TYPE::USER_DATA, Gamnet::Format(i + 1));
		LOG(INF, "[Redis] user_id:", value);
	}

	ReadXml(config_path);

	for (int i = 0; i < 100; i++)
	{
		Set(DATABASE_TYPE::CONFIG_DB_ID, Gamnet::Format(i + 1), Gamnet::Format("USER_", i + 1));
		std::string value = Get(DATABASE_TYPE::CONFIG_DB_ID, Gamnet::Format(i + 1));
		LOG(INF, "[Redis] user_id:", value);
	}

}

int main(int argc, char** argv) 
{
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

	Gamnet::Log::ReadXml(config_path);
	LOG(INF, "005.Database Server Starts config=", config_path);

	Gamnet::InitCrashDump();
	try {
		Gamnet::Log::ReadXml(config_path);
		LOG(INF, argv[0], " Server Starts..");
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Database::MySQL::ReadXml(config_path);
		Gamnet::Database::SQLite::ReadXml(config_path);
		Gamnet::Database::Redis::ReadXml(config_path);

		Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
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


