/*
 * DatabaseImpl.h
 *
 *  Created on: Jul 24, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_DATABASEIMPL_H_
#define GAMNET_DATABASE_DATABASEIMPL_H_

#include "ResultSet.h"
#include "../Library/Pool.h"
namespace Gamnet { namespace Database {

enum ERRNO
{
	ERRNO_SUCCESS = 0,
	ERRNO_NO_CONNECTIONINFO_ERROR,
	ERRNO_DUPLICATE_CONNECIONINFO_ERROR,
	ERRNO_ALLOC_OBJECT_ERROR,
	ERRNO_EXECUTE_QUERY_ERROR,
};

class DatabaseImpl
{
	typedef Pool<sql::Connection, std::mutex> ConnectionPool;
	struct ConnectionInfo
	{
		const std::string uri_;
		const std::string id_;
		const std::string passwd_;
		const std::string db_;
	};

	sql::mysql::MySQL_Driver* driver_;
	std::map<int, std::shared_ptr<ConnectionPool>> mapConnectionPool_;
	std::map<int, ConnectionInfo> mapConnectionInfo_;

	sql::Connection* ConnectionFactory(const ConnectionInfo& connectionInfo);
public :
	DatabaseImpl();
	~DatabaseImpl();
	bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);
	ResultSet Execute(int db_type, const std::string& query, std::function<void(ResultSet)> callback = [](ResultSet){});
};

}}
#endif /* DATABASEIMPL_H_ */
