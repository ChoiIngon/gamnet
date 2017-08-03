#include "Database.h"

namespace Gamnet {	namespace Database {
	void ReadXml(const char* xml_path)
	{
		MySQL::ReadXml(xml_path);
	}

	bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db)
	{
		return MySQL::Connect(db_type, host, port, id, passwd, db);
	}
}}