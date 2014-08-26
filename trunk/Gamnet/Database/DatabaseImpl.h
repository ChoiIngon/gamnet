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
#include "Connection.h"

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
	typedef Pool<Connection, std::mutex> ConnectionPool;
	std::map<int, std::shared_ptr<ConnectionPool>> mapConnectionPool_;
	std::map<int, Connection::ConnectionInfo> mapConnectionInfo_;

	Connection* ConnectionFactory(const Connection::ConnectionInfo& connInfo);
public :
	DatabaseImpl();
	~DatabaseImpl();
	bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);
	ResultSet Execute(int db_type, const std::string& query, std::function<void(ResultSet)> callback = [](ResultSet){});
};

}}
#endif /* DATABASEIMPL_H_ */
