# Gamnet/Database

- 'gamnet'은 MySQL과 Redis를 지원합니다.

# Features

- connection pool 

# Usage
## MySQL
* There are two way for connection to MySQL. Using 'Connect' function directly or 'ReadXml' function reading configueration from xml file.
### Using 'Connect' function directly
```cpp
#include <Gamnet.h>

int main()
{
	// 'Gamnet::Database::MySQL' uses internally 'Gamnet::Log'. so before calling 'Connect' function, you should init Log
	Gamnet::Log::ReadXml("config.xml");
	
	int db_type = 1; // id for identify database connection pool
	const char* host = "some.host.com"; // domain name or ip of mysql server
	int port = 3306; // mysql server port
	const char* id = "user_id"; // mysql user id
	const char* passwd = "user_passwd"; // mysql user password
	const char* db = "db_name"; // database name
	
	Gamnet::Database::MySQL::Connect(db_type, host, port, id, passwd, db);
	return 0;	
}
```
### Using config file fo connection
config.xml
```xml
<?xml version="1.0" encoding="UTF-8" ?>
<server>
	<log path="log" prefix="sample" max_file_size="5">
		<dev console="yes" file="yes" sys="no"/>
		<inf console="yes" file="yes" sys="no"/>
		<err console="yes" file="yes" sys="no"/>
	</log>
	<!--
		name : human readable text
		id : id for identify database connection pool <integer>
		host : domain name or ip of mysql server <string>
		port : mysql server port <integer>
		user : mysql user id <string>
		passwd : mysql user password <string>
		db : database name <string>
	-->
	<database name="DB_NAME_1" id="1" host="some.host.com" port="3306" user="user_id" passwd="user_passwd" db="db_name_1" />
	<database name="DB_NAME_2" id="2" host="some.host.com" port="3306" user="user_id" passwd="user_passwd" db="db_name_2" />
<server/>
```

```cpp
#include <Gamnet.h>

int main()
{
	// 'Gamnet::Database::MySQL' uses internally 'Gamnet::Log'. so before calling 'Connect' function, you should init Log
	Gamnet::Log::ReadXml("config.xml");
	Gamnet::Database::MySQL::ReadXml("config.xml");
	return 0;	
}
```
### Query
```cpp
#include <Gamnet.h>
#include <string>
#include <iostream>

int main()
{
	// 'Gamnet::Database::MySQL' uses internally 'Gamnet::Log'. so before calling 'Connect' function, you should init Log
	Gamnet::Log::ReadXml("config.xml");
	Gamnet::Database::MySQL::ReadXml("config.xml");
	
	int db_type = 1;
	int user_type = 1;
	std::string user_addr = "Korea";

	// select
	Gamnet::Database::MySQL::ResultSet select = Gamnet::Database::MySQL::Execute(db_type,
		"select USER_SEQ, USER_ID, USER_NAME from USER where USER_TYPE=", user_type, " and USER_ADDR='", user_addr, "'"
	);
	
	for(auto row = select.begin(); row != select.end(); row++)
	{
		std::cout << "user seq:" << row->getInt("USER_SEQ") << std::endl;
		std::cout << "user id:" << row->getString("USER_ID") << std::endl;
		std::cout << "user name:" << row->getString("USER_NAME") << std::endl;
	}
	
	// update
	Gamnet::Database::MySQL::ResultSet update = Gamnet::Database::MySQL::Execute(db_type,
		"update USER set USER_ADDR='USA' where USER_TYPE=", user_type, " and USER_ADDR='", user_addr, "'"
	);
	
	std::cout << update.GetAffectedRow() << std::endl;
	
	// insert
	Gamnet::Database::MySQL::ResultSet insert = Gamnet::Database::MySQL::Execute(db_type,
		"insert into USER(USER_ID, USER_NAME, USER_TYPE, USER_ADDR) values('id', 'name', 'type', 'Korea')"
	);
	
	std::cout << insert.GetLastInsertID() << std::endl;
	
	return 0;	
}
```
## Redis
