/*
 * Database.cpp
 *
 *  Created on: Jul 24, 2013
 *      Author: kukuta
 */

#include "Database.h"
#include "../Library/ThreadPool.h"
#include "../Library/Singleton.h"
namespace Gamnet { namespace Database {

static ThreadPool	  threadPool_(16);

bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db)
{
	return Singleton<DatabaseImpl>().Connect(db_type, host, port, id, passwd, db);
}

ResultSet Execute(int db_type, const std::string& query)
{
	return Singleton<DatabaseImpl>().Execute(db_type, query);
}

void AExecute(int db_type, const std::string& query, std::function<void(ResultSet)> callback)
{
	threadPool_.PostTask(std::bind(&DatabaseImpl::Execute, &Singleton<DatabaseImpl>(), db_type, query, callback));
}

}}


