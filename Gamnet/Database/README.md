# Gamnet/Database

- 'gamnet'은 MySQL과 Redis를 지원합니다.

# Features

- connection pool 

# Usage
## MySQL
* There are two way for connection to MySQL. Using 'Connect' function directly or 'ReadXml' function reading configueration from xml file.
### Direct 'Connect' function
```cpp
#include <Gamnet.h>

int main()
{
	// 'Gamnet::Database::MySQL' uses internally 'Gamnet::Log'. so before calling 'Connect' function, you should init Log
	Gamnet::Log::ReadXml("config.xml");
	
	int db_type = 1; // id for identify database connection pool
	const char* host = "some.host.com"; // domain name or ip of mysql server
	int port = 3306; // mysql server port
	const char* id = "mysql_user_id"; // mysql user id
	const char* passwd = "mysq_user_passwd"; // mysql user password
	const char* db = "db_name"; // database name
	
	Gamnet::Database::MySQL::Connect(db_type, host, port, "mysql_id", "mysql_password", "db_name");
	return 0;	
}
```
### Query

## Redis
