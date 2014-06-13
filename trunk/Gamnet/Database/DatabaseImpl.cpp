/*
 * DatabaseImpl.cpp
 *
 *  Created on: Jul 24, 2013
 *      Author: kukuta
 */

#include "DatabaseImpl.h"
#include <boost/algorithm/string.hpp>
#include <error.h>
#include "../Log/Log.h"
#include "../Library/String.h"

namespace Gamnet { namespace Database {

DatabaseImpl::DatabaseImpl(): driver_(NULL)
{
	Log::Init();
	driver_ = sql::mysql::get_mysql_driver_instance();
}

DatabaseImpl::~DatabaseImpl()
{
}

sql::Connection* DatabaseImpl::ConnectionFactory(const ConnectionInfo& connectionInfo)
{
	if(NULL == driver_)
	{
		Log::Write(GAMNET_ERR, "driver is not inited(uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,")");
		return NULL;
	}
	sql::Connection* connection = NULL;
	try {
		sql::ConnectOptionsMap connection_properties;
		connection_properties["hostName"] = connectionInfo.uri_;
		connection_properties["userName"] = connectionInfo.id_;
	    connection_properties["password"] = connectionInfo.passwd_;
		connection_properties["OPT_RECONNECT"] = true;
		connection_properties["OPT_CONNECT_TIMEOUT"] = 3;
		connection = driver_->connect(connection_properties);
		connection->setSchema(connectionInfo.db_.c_str());
		Log::Write(GAMNET_INF, "database connected(uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,")");
	}
	catch (const sql::SQLException& e)
	{
		Log::Write(GAMNET_ERR, "database connect error(uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,", reason:", e.what(), ")");
		errno = e.getErrorCode();
		if(NULL != connection)
		{
			delete connection;
			connection = NULL;
		}
	}
	return connection;
}

bool DatabaseImpl::Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db)
{
	std::string&& uri = Format("tcp://", host, ":", port);
	const ConnectionInfo connectionInfo = {uri, id, passwd, db};
	if(NULL == driver_)
	{
		Log::Write(GAMNET_ERR, "invalid connection info(db_type:", db_type, ", uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,")");
		return false;
	}

	if(false == mapConnectionInfo_.	insert(std::make_pair(db_type, connectionInfo)).second)
	{
		errno = ERRNO_DUPLICATE_CONNECIONINFO_ERROR;
		Log::Write(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ", uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,")");
		return false;
	}
	if(false == mapConnectionPool_.insert(std::make_pair(db_type, std::shared_ptr<ConnectionPool>(new ConnectionPool(64, std::bind(&DatabaseImpl::ConnectionFactory, this, connectionInfo))))).second)
	{
		errno = ERRNO_DUPLICATE_CONNECIONINFO_ERROR;
		Log::Write(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ", uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,")");
		return false;
	}

	// connect test
	std::shared_ptr<sql::Connection> connection = mapConnectionPool_[db_type]->Create();
	if(NULL == connection)
	{
		Log::Write(GAMNET_ERR, "connection fail(db_type:", db_type, ", uri:", connectionInfo.uri_, ", id:", connectionInfo.id_, ", db:", connectionInfo.db_,")");
		return false;
	}
	return true;
}

ResultSet DatabaseImpl::Execute(int db_type, const std::string& query, std::function<void(ResultSet)> callback)
{
	ResultSet res;
	errno = ERRNO_SUCCESS;

	auto itr = mapConnectionPool_.find(db_type);
	if(mapConnectionPool_.end() == itr)
	{
		Log::Write(GAMNET_ERR, "can't find database connection(db_type:", db_type, ")");
		errno = ERRNO_NO_CONNECTIONINFO_ERROR;
		return res;
	}

	std::shared_ptr<sql::Connection> connection = itr->second->Create();
	if(NULL == connection)
	{
		Log::Write(GAMNET_ERR, "create Connection object error(db_type:", db_type, ")");
		errno = ERRNO_ALLOC_OBJECT_ERROR;
		return res;
	}

	sql::Statement* stmt = connection->createStatement();
	if(NULL == stmt)
	{
		Log::Write(GAMNET_ERR, "create Statement object error(db_type:", db_type, ")");
		errno = ERRNO_ALLOC_OBJECT_ERROR;
		return res;
	}
	try {
		bool isSelectQuery = stmt->execute(query);
		if(true == isSelectQuery)
		{
			res.resultSet_.reset(stmt->getResultSet());
		}
		else
		{
			res.affectedRowCount_ = stmt->getUpdateCount();
		}
	}
	catch (sql::SQLException& e)
	{
		Log::Write(GAMNET_ERR, "query error(query:", query, ", reason:", e.what(), ", error_code:", e.getErrorCode(), ", state:", e.getSQLState(), ")");
		errno = ERRNO_EXECUTE_QUERY_ERROR;
		res.errno_ = e.getErrorCode();
		if(1046 == e.getErrorCode()) // No database selected
		{
			const ConnectionInfo& connectionInfo = mapConnectionInfo_[db_type];
			connection->setSchema(connectionInfo.db_.c_str());
		}
	}
	stmt->getMoreResults();
	delete stmt;
	callback(res);
	return res;
}

}}
