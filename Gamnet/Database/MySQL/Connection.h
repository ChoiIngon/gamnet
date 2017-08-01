/*
 * Connection.h
 *
 *  Created on: Aug 25, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_MYSQL_CONNECTION_H_
#define GAMNET_DATABASE_MYSQL_CONNECTION_H_

#ifdef _WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif
#include <string>
#include "ResultSet.h"

namespace Gamnet { namespace Database {namespace MySQL {
	class Connection {
	public:
		struct ConnectionInfo
		{
			std::string uri_;
			std::string id_;
			std::string passwd_;
			std::string db_;
			std::string charset_;
			bool reconnect_;
			unsigned int timeout_;
			int port_;

			ConnectionInfo() : charset_("utf8"), reconnect_(true), timeout_(5), port_(3306)
			{
			}
		};
		MYSQL conn_;
		ConnectionInfo connInfo_;

		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
	};
} } }

#endif /* CONNECTION_H_ */
