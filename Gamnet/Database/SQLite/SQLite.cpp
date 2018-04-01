#include "SQLite.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace SQLite {
	bool Connect(int db_type, const char* db)
	{
		Connection::ConnectionInfo connInfo;
		connInfo.db_ = db;
		return Singleton<Database::ConnectionPool<Connection>>::GetInstance().Connect(db_type, connInfo);
	}

	ResultSet Execute(int db_type, const std::string& query)
	{
		std::shared_ptr<Connection> conn = Singleton<Database::ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
		ResultSet res;
		res.impl_ = conn->Execute(query);
		res.impl_->conn_ = conn;
		return res;
	}
}}}