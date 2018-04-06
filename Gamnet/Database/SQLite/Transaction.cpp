#include "Transaction.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database {	namespace SQLite {

	Transaction::Transaction(int db_type) : commit(false)
	{
		connection = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
		connection->Execute("begin transaction");
	}

	Transaction::~Transaction() {
		if (false == commit && nullptr != connection)
		{
			connection->Execute("rollback");
		}
	}

	ResultSet Transaction::Commit()
	{
		ResultSet ret;
		ret.impl_ = connection->Execute("commit transaction");
		ret.impl_->conn_ = connection;
		connection = nullptr;
		commit = true;
		return ret;
	}
}}}