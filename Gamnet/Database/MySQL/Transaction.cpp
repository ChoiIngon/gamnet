#include "Transaction.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace MySQL {

Transaction::Transaction(int db_type) : commit(false)
{
	connection = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
}

ResultSet Transaction::Commit()
{
	ResultSet res;
	if ("" != queries)
	{
		res.impl_ = connection->Execute("start transaction;" + queries + " commit;");
		try {
			while (res.NextResult());
		}
		catch (const Gamnet::Exception& e)
		{
			// CR_CONN_HOST_ERROR
			if(nullptr != connection->logger_)
			{
				connection->logger_->Write(Log::Logger::LOG_LEVEL_INF, "start transaction; " + queries + " commit;");
			}
			throw e;
		}
	}
	
	commit = true;
	return res;
}

Transaction::~Transaction() 
{
	if(false == commit)
	{
		connection->Execute("rollback");
	}
}

} /* namespace Database */
} /* namespace Gamnet */
}