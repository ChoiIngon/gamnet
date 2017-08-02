# Gamnet/Database

- 'gamnet'은 MySQL과 Redis를 지원합니다.

# Features

- connection pool 

# Usage
## MySQL
### Connect
```cpp
namespace Gamnet { namespace Database { namespace MySQL {
	void ReadXml(const char* xml_path);
	bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);
	ResultSet Execute(int db_type, const std::string& query);
	template <class... ARGS>
	ResultSet Execute(int db_type, ARGS... args)
	{
		return Execute(db_type, Format(args...));
	}
}}}
```
### Query

## Redis
