/*
 * Connection.h
 *
 *  Created on: Aug 25, 2014
 *      Author: kukuta
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <mysql/mysql.h>
#include <string>
#include "ResultSet.h"

namespace Gamnet {
namespace Database {

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

		ConnectionInfo() :
			charset_("utf8"), reconnect_(true), timeout_(5), port_(3306)
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

} /* namespace Database */
} /* namespace Gamnet */

#endif /* CONNECTION_H_ */
