#ifndef _GAMNET_DATABASE_SQLITE_CONNECTION_H
#define _GAMNET_DATABASE_SQLITE_CONNECTION_H

#include "sqlite3.h"
#include "ResultSet.h"
#include <string>
#include <mutex>
#include <condition_variable>

namespace Gamnet { namespace Database { namespace SQLite {
	struct UnlockNotification 
	{
		UnlockNotification();
		bool fired;
		std::mutex lock;
		std::condition_variable cond_var;
	};

	class Connection {
	public :
		struct ConnectionInfo
		{
			ConnectionInfo();
			std::string db_;
			int timeout_;
		};

		sqlite3* conn_;
		ConnectionInfo connInfo_;
		UnlockNotification un_;

		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
	};

	void UnlockNotifyCallback(void** argv, int argc);
	int WaitForUnlockNotify(Connection* conn);
	int ExecCallback(void* arg, int columnCount, char **rowDatas, char **columnName);
}}}
#endif