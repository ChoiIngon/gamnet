#include "Transaction.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace Redis {

Transaction::Transaction(int db_type) : commit(false)
{
	connection = Singleton<ConnectionPool<Connection, ResultSet>>::GetInstance().GetConnection(db_type);
	ResultSet res;
	res.impl_ = connection->Execute("MULTI");
}

Transaction::~Transaction() {
	if (false == commit && nullptr != connection)
	{
		connection->Execute("DISCARD");
	}
}

ResultSet Transaction::Commit()
{
	ResultSet ret;
	ret.impl_ = connection->Execute("EXEC");
	ret.impl_->conn_ = connection;
	connection = nullptr;
	commit = true;
	return ret;
}

} /* namespace Database */ } /* namespace Gamnet */ }