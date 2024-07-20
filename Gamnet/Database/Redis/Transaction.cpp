#include "Transaction.h"
#include "../ConnectionPool.h"

import Gamnet.Singleton;

namespace Gamnet { namespace Database { namespace Redis {

Transaction::Transaction(int db_type) : commit(false)
{
	connection = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
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
	connection = nullptr;
	commit = true;
	return ret;
}

} /* namespace Database */ } /* namespace Gamnet */ }