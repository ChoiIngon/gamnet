/*
 * Connection.h
 *
 *  Created on: Aug 25, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_MYSQL_CONNECTION_H_
#define GAMNET_DATABASE_MYSQL_CONNECTION_H_

#include <mysql.h>
#include <string>
#include "ResultSet.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database {namespace MySQL {
	class Connection : public std::enable_shared_from_this<Connection>
	{
		bool Reconnect();
	public:
		struct ConnectionInfo
		{
			std::string uri_;
			std::string id_;
			std::string passwd_;
			std::string db_;
			std::string charset_;
			int db_type_;
			bool reconnect_;
			unsigned int timeout_;
			int port_;
			bool fail_query_log_;
		
			ConnectionInfo() : charset_("utf8"), db_type_(0), reconnect_(true), timeout_(5), port_(3306), fail_query_log_(false)
			{
			}
		};
		MYSQL conn_;
		ConnectionInfo connInfo_;
		Log::Logger* logger_;
		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
		std::string RealEscapeString(const std::string& str);
	};
} } }

#endif /* CONNECTION_H_ */
