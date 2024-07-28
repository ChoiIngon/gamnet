#include "Transaction.h"
#include <Gamnet/Gamnet.h>
#include <Gamnet/Database/ConnectionPool.h>

Transaction::Transaction(int shard_index)
    : shard_index(shard_index)
{
}

bool Transaction::operator()(const std::shared_ptr<Statement>& statement)
{
    statements.push_back(statement);
    return true;
}

void Transaction::Commit()
{
	typedef Gamnet::Database::MySQL::Connection Connection;
	typedef Gamnet::Database::ConnectionPool<Connection> ConnectionPool;
	if (0 == statements.size())
	{
		return;
	}

	std::shared_ptr<Connection> connection = Gamnet::Singleton<ConnectionPool>::GetInstance().GetConnection(shard_index);

	connection->Execute("start transaction;");
	try {
		for (auto statement : statements)
		{
			statement->Commit(connection);
		}
	}
	catch (const Gamnet::Exception& e)
	{
		connection->Execute("rollback;");
		Rollback();
		throw GAMNET_EXCEPTION(e.error_code(), e.what());
	}
	connection->Execute("commit;");

	for (auto statement : statements)
	{
		statement->Sync();
	}
}

void Transaction::Rollback()
{
	for (auto statement : statements)
	{
		statement->Rollback();
	}
}