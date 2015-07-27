/*
 * Transaction.cpp
 *
 *  Created on: Jul 24, 2015
 *      Author: wered
 */

#include "Transaction.h"
#include "DatabaseImpl.h"
#include "../Library/Singleton.h"
#include "../Library/Exception.h"

namespace Gamnet {
namespace Database {

Transaction::Transaction(int db_type) : commit(false)
{
	connection = Singleton<DatabaseImpl>::GetInstance().GetConnection(db_type);
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
void Transaction::Commit()
{
	connection->Execute("commit");
	connection = NULL;
	commit = true;
}

Transaction::~Transaction() {
	if(false == commit && NULL != connection)
	{
		connection->Execute("rollback");
	}
}

} /* namespace Database */
} /* namespace Gamnet */
