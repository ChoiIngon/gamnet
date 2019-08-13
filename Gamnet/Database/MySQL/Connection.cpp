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
#include "../../Log/Log.h"

namespace Gamnet {	namespace Database { namespace MySQL {
	static std::map<int, Log::Logger> logger_manager;

	Connection::Connection() : logger_(nullptr)
	{
	}

	Connection::~Connection()
	{
		mysql_close(&conn_);
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		connInfo_ = connInfo;
		if (NULL == mysql_init(&conn_)) // mysql_thread_init() automatically called
		{
			return false;
		}

		mysql_options(&conn_, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&connInfo_.timeout_);
		mysql_options(&conn_, MYSQL_OPT_RECONNECT, (const char*)&connInfo_.reconnect_);
		mysql_options(&conn_, MYSQL_SET_CHARSET_NAME, connInfo.charset_.c_str());
		mysql_options(&conn_, MYSQL_INIT_COMMAND, Format("SET NAMES '", connInfo.charset_, "';").c_str());
		LOG(INF, "[MySQL] connect...(host:", connInfo.uri_, ", port:", connInfo.port_, ", db:", connInfo.db_, ", user:", connInfo.id_, /*", passwd:", connInfo.passwd_, */")");
		if (nullptr == mysql_real_connect(&conn_, connInfo.uri_.c_str(), connInfo.id_.c_str(), connInfo.passwd_.c_str(), connInfo.db_.c_str(), connInfo.port_, nullptr, CLIENT_MULTI_STATEMENTS))
		{
			LOG(GAMNET_ERR, "[MySQL] connect fail(host:", connInfo.uri_, ", port:", connInfo.port_, ", error_message:", mysql_error(&conn_),")");
			mysql_thread_end();
			return false;
		}

		
		auto itr = logger_manager.find(connInfo_.db_type_);
		if(logger_manager.end() == itr)
		{
			auto& logger = logger_manager[connInfo_.db_type_];
			logger.Init("db_error", connInfo_.db_.c_str(), 5);
			logger.SetTimeRecord(false);
			logger.SetLevelProperty(Log::Logger::LOG_LEVEL_INF, Log::Logger::LOG_FILE);
			logger_ = &logger;
		}

		return true;
	}

	bool Connection::Reconnect()
	{
		mysql_close(&conn_);
		return Connect(connInfo_);
	}

	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{
		std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl(shared_from_this()));
		try {
			impl->Execute(query);
		}
		catch(const Exception& e)
		{
			if(nullptr != logger_)
			{
				logger_->Write(Log::Logger::LOG_LEVEL_INF, query, ";");
			}
			Reconnect();
			throw e;
		}

		return impl;
	}

	std::string Connection::RealEscapeString(const std::string& str)
	{
		std::vector<char> buff(str.length() * 2 + 1);
		unsigned long length = mysql_real_escape_string(&conn_, &buff[0], str.c_str(), str.length());
		return std::string(&buff[0], length);
	}

} /* namespace Database */ } /* namespace Gamnet */ }
