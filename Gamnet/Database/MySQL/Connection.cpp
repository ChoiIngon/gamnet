/*
 * Connection.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: kukuta
 */

#include "Connection.h"
#include <boost/algorithm/string.hpp>
#include "../../Library/Exception.h"
#include "../../Library/String.h"
#include "../../Library/ThreadPool.h"
#include "../../Log/Log.h"

namespace Gamnet {	namespace Database { namespace MySQL {
	static std::map<int, Log::Logger> logger_manager;
	//static ThreadPool thread_pool(30);

	Connection::ConnectionInfo::ConnectionInfo()
		: db_num(0)
		, host("")
		, port(3306)
		, userid("")
		, password("")
		, database("")
		, charset("utf8mb4")
		, timeout(0)
		, fail_query_log(false)
	{
	}

	/*
	Connection* Connection::ReleaseFunctor::operator()(Connection* conn)
	{
		conn->Release();
		return conn;
	}
	*/

	Connection::Connection() 
		: mysql_conn(MYSQL())
		, fail_query_logger(nullptr)
	{
	}

	Connection::~Connection()
	{
		mysql_close(&mysql_conn);
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		conn_info = connInfo;
		if (nullptr == mysql_init(&mysql_conn)) // mysql_thread_init() automatically called
		{
			return false;
		}

		bool reconnectFlag = true;
		mysql_options(&mysql_conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&conn_info.timeout);
		mysql_options(&mysql_conn, MYSQL_OPT_RECONNECT, (bool*)&reconnectFlag);
		mysql_options(&mysql_conn, MYSQL_SET_CHARSET_NAME, conn_info.charset.c_str());
		//mysql_options(&mysql_conn, MYSQL_INIT_COMMAND, Format("SET NAMES '", conn_info.charset, "';").c_str());
		LOG(INF, "[Gamnet::Database::MySQL] connect...(db_num:", conn_info.db_num, ", host:", conn_info.host, ", port:", conn_info.port, ", db:", conn_info.database, ", user:", connInfo.userid, /*", passwd:", connInfo.passwd_, */")");
		if (nullptr == mysql_real_connect(&mysql_conn, conn_info.host.c_str(), conn_info.userid.c_str(), conn_info.password.c_str(), conn_info.database.c_str(), conn_info.port, nullptr, CLIENT_MULTI_STATEMENTS))
		{
			LOG(GAMNET_ERR, "[Gamnet::Database::MySQL] connect fail(host:", conn_info.host, ", port:", conn_info.port, ", error_message:", mysql_error(&mysql_conn),")");
			mysql_thread_end();
			return false;
		}
		
		if(true == conn_info.fail_query_log)
		{
			auto itr = logger_manager.find(conn_info.db_num);
			if(logger_manager.end() == itr)
			{
				auto& logger = logger_manager[conn_info.db_num];
				logger.Init("db_error", conn_info.database.c_str(), 5);
				logger.SetTimeRecord(false);
				logger.SetLevelProperty(Log::Logger::LOG_LEVEL_INF, Log::Logger::LOG_FILE);
			}

			auto& logger = logger_manager[conn_info.db_num];
			fail_query_logger = &logger;
		}
		return true;
	}

	/*
	bool Connection::Reconnect()
	{
		if(0 == mysql_ping(&mysql_conn))
		{
			return true;
		}
		mysql_close(&mysql_conn);
		return Connect(conn_info);
	}
	*/
	
	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{
		std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl(shared_from_this()));
		try {
			if (0 != mysql_real_query(&mysql_conn, query.c_str(), query.length()))
			{
				impl->error_code = mysql_errno(&mysql_conn);
				throw Exception(ErrorCode::ExecuteQueryError, "query:", query, ", error:", mysql_error(&mysql_conn), ", errno:", impl->error_code);
			}

			if (false == impl->StoreResult())
			{
				throw Exception(ErrorCode::ExecuteQueryError, "query:", query, ", error:", mysql_error(&mysql_conn), ", errno:", impl->error_code);
			}
		}
		catch(const Exception& e)
		{
			LOG(GAMNET_ERR, impl->error_code, "(query:", query, ")");
			if(nullptr != fail_query_logger)
			{
				fail_query_logger->Write(Log::Logger::LOG_LEVEL_INF, query, ";");
			}
			//Reconnect();
			throw e;
		}

		return impl;
	}

	/*
	void Connection::AsyncExecute(const std::string& query, const std::function<void(const std::shared_ptr<ResultSetImpl>)>& callback)
	{
		auto self = shared_from_this();
		thread_pool.PostTask([this, self, query, callback]() {
			std::shared_ptr<ResultSetImpl> impl(std::make_shared<ResultSetImpl>(self));
			try {
				if (0 != mysql_real_query(&mysql_conn, query.c_str(), query.length()))
				{
					impl->error_code = mysql_errno(&mysql_conn);
					throw Exception(ErrorCode::ExecuteQueryError, "query:", query, ", error:", mysql_error(&mysql_conn), ", errno:", impl->error_code);
				}

				if (false == impl->StoreResult())
				{
					throw Exception(ErrorCode::ExecuteQueryError, "query:", query, ", error:", mysql_error(&mysql_conn), ", errno:", impl->error_code);
				}
			}
			catch (const Exception& e)
			{
				LOG(GAMNET_ERR, impl->error_code, "(query:", query, ")");
				if (nullptr != fail_query_logger)
				{
					fail_query_logger->Write(Log::Logger::LOG_LEVEL_INF, query, ";");
				}
				
			}
			callback(impl);
		});
	}
	*/

	std::string Connection::RealEscapeString(const std::string& str)
	{
		std::vector<char> buff(str.length() * 2 + 1);
		unsigned long length = mysql_real_escape_string(&mysql_conn, &buff[0], str.c_str(), str.length());
		return std::string(&buff[0], length);
	}

	void Connection::Release()
	{
		// trash remain result
		if (mysql_more_results(&mysql_conn))
		{
			while (0 == mysql_next_result(&mysql_conn))
			{
				MYSQL_RES* res = mysql_store_result(&mysql_conn);
				if (nullptr != res)
				{
					mysql_free_result(res);
				}
			}
		}
	}

	/*
	bool ConnectionManager::Connect(int db_type, const Connection::ConnectionInfo& connInfo)
	{
		if (false == conn_infos.insert(std::make_pair(db_type, connInfo)).second)
		{
			LOG(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ")");
			return false;
		}
		
		std::shared_ptr<CONN_POOL_T> conn_pool(std::make_shared<CONN_POOL_T>(64, std::bind(&ConnectionManager::ConnectionFactory, this, db_type)));
		if (false == conn_pools.insert(std::make_pair(db_type, conn_pool)).second)
		{
			LOG(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ")");
			return false;
		}

		// connect test
		std::shared_ptr<Connection> conn = conn_pools[db_type]->Create();
		if (nullptr == conn)
		{
			LOG(GAMNET_ERR, "connection fail(db_type:", db_type, ")");
			return false;
		}
		return true;
	}

	std::shared_ptr<Connection> ConnectionManager::GetConnection(int db_type)
	{
		auto itr = conn_pools.find(db_type);
		if (conn_pools.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidDatabaseNum, "can't find database connection(db_type:", db_type, ")");
		}

		std::shared_ptr<Connection> conn(itr->second->Create());
		if (nullptr == conn)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidDatabaseNum, "create Connection object error(db_type:", db_type, ")");
		}
		return conn;
	}

	Connection* ConnectionManager::ConnectionFactory(int db_type)
	{
		auto itr = conn_infos.find(db_type);
		if (conn_infos.end() == itr)
		{
			LOG(GAMNET_ERR, "can't find connection information for db(db_type:", db_type, ")");
			throw Exception(0, "can't find connection information for db(db_type:", db_type, ")");
		}

		Connection* conn = new Connection();
		Connection::ConnectionInfo& connInfo = itr->second;
		if (false == conn->Connect(connInfo))
		{
			delete conn;
			return nullptr;
		}
		return conn;
	}
	*/
} /* namespace Database */ } /* namespace Gamnet */ }
