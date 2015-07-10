/*
 * DatabaseImpl.cpp
 *
 *  Created on: Jul 24, 2013
 *      Author: kukuta
 */

#include "DatabaseImpl.h"
#include <boost/algorithm/string.hpp>
#include "../Log/Log.h"
#include "../Library/String.h"

namespace Gamnet { namespace Database {

DatabaseImpl::DatabaseImpl()
{
}

DatabaseImpl::~DatabaseImpl()
{
}

Connection* DatabaseImpl::ConnectionFactory(const Connection::ConnectionInfo& connInfo)
{
	Connection* conn = new Connection();
	if(false == conn->Connect(connInfo))
	{
		delete conn;
		return NULL;
	}
	return conn;
}

bool DatabaseImpl::Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db)
{
	Connection::ConnectionInfo connInfo;
	connInfo.uri_ = host;
	connInfo.id_ = id;
	connInfo.passwd_ = passwd;
	connInfo.db_ = db;
	connInfo.port_ = port;

	if(false == mapConnectionInfo_.insert(std::make_pair(db_type, connInfo)).second)
	{
		Log::Write(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ", uri:", connInfo.uri_, ", id:", connInfo.id_, ", db:", connInfo.db_,")");
		return false;
	}
	if(false == mapConnectionPool_.insert(std::make_pair(db_type, std::shared_ptr<ConnectionPool>(new ConnectionPool(64, std::bind(&DatabaseImpl::ConnectionFactory, this, connInfo))))).second)
	{
		Log::Write(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ", uri:", connInfo.uri_, ", id:", connInfo.id_, ", db:", connInfo.db_,")");
		return false;
	}

	// connect test
	std::shared_ptr<Connection> connection = mapConnectionPool_[db_type]->Create();
	if(NULL == connection)
	{
		Log::Write(GAMNET_ERR, "connection fail(db_type:", db_type, ", uri:", connInfo.uri_, ", id:", connInfo.id_, ", db:", connInfo.db_,")");
		return false;
	}
	return true;
}

ResultSet DatabaseImpl::Execute(int db_type, const std::string& query, std::function<void(ResultSet)> callback)
{
	auto itr = mapConnectionPool_.find(db_type);
	if(mapConnectionPool_.end() == itr)
	{
#ifdef _WIN32
		throw Exception(GAMNET_ERROR_DB_INVALID_CONNECTION_POOL_TYPE, "ERR [", __FILE__, ":", __FUNCTION__, "@", __LINE__, "] can't find database connection(db_type:", db_type, ")");
#else
		throw Exception(GAMNET_ERROR_DB_INVALID_CONNECTION_POOL_TYPE, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] can't find database connection(db_type:", db_type, ")");
#endif
	}

	std::shared_ptr<Connection> connection(itr->second->Create());
	if(NULL == connection)
	{
		LOG(GAMNET_ERR, "create Connection object error(db_type:", db_type, ")");
		throw Exception(GAMNET_ERROR_DB_CONNECT_FAIL, ERR, "create Connection object error(db_type:", db_type, ")");
	}
	ResultSet res;
	res.impl_ = connection->Execute(query);
	res.impl_->conn_ = connection;
	return res;
}

}}
