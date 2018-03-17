#ifndef GAMNET_DATABASE_DATABASE_H
#define GAMNET_DATABASE_DATABASE_H

#include "MySQL/MySQL.h"
#include "Redis/Redis.h"
#include "SQLite/SQLite.h"

namespace Gamnet {	namespace Database {
	typedef MySQL::ResultSet ResultSet;
	typedef MySQL::Transaction Transaction;
	void ReadXml(const char* xml_path);
	bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);

	template <class... ARGS>
	ResultSet Execute(int db_type, ARGS... args)
	{
		return MySQL::Execute(db_type, Format(args...));
	}
}}
#endif
