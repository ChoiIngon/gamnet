#ifndef _GAMNET_DATABASE_SQLITE_CONNECTION_H
#define _GAMNET_DATABASE_SQLITE_CONNECTION_H

#include "sqlite3.h"
#include <string>
#include "ResultSet.h"

namespace Gamnet { namespace Database { namespace SQLite {
	class Connection {
	public :
		struct ConnectionInfo
		{
			std::string db_;
		};

		sqlite3* conn_;
		ConnectionInfo connInfo_;

		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);

		static int ExecCallback(void *arg, int columnCount, char **rowDatas, char **columnName);
	};
}}}
#endif