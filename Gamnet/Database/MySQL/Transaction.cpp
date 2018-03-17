#include "Transaction.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace MySQL {

Transaction::Transaction(int db_type) : commit(false)
{
	connection = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	connection->Execute("start transaction");
}
/*
Transaction::Transaction(int db_type, std::function<void(Transaction& transaction)> t)
{
	connection = Singleton<DatabaseImpl>::GetInstance().GetConnection(db_type);
	try {
		connection->Execute("start transaction");
		t(*this);
		connection->Execute("commit");
	}
	catch (const Exception& e)
	{
		connection->Execute("rollback");
		connection = NULL;
		throw e;
	}
	connection = NULL;
}
*/
ResultSet Transaction::Commit()
{
	ResultSet ret;
	ret.impl_ = connection->Execute("commit");
	ret.impl_->conn_ = connection;
	connection = nullptr;
	commit = true;
	return ret;
}

Transaction::~Transaction() {
	if(false == commit && nullptr != connection)
	{
		connection->Execute("rollback");
	}
}

} /* namespace Database */
} /* namespace Gamnet */
}