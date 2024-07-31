#include "Transaction.h"
#include <Gamnet/Gamnet.h>
#include <Gamnet/Database/ConnectionPool.h>
#include "../UserSession.h"

Transaction::Transaction(const std::shared_ptr<UserSession>& session)
    : session(session)
{
}
bool Transaction::operator()(const std::shared_ptr<Statement>& statement)
{
	statements.push_back(statement);
	return true;
}

bool Transaction::operator()(const Gamnet::Return<std::shared_ptr<Statement>>& result)
{
    if (false == result)
    {
        return false;
    }

    statements.push_back(result.value);
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

	std::shared_ptr<Connection> connection = Gamnet::Singleton<ConnectionPool>::GetInstance().GetConnection(session->shard_index);

	connection->Execute("start transaction;");
	try {
		for (auto statement : statements)
		{
			statement->Commit(session, connection);
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
		statement->Sync(session);
	}
}

void Transaction::Rollback()
{
	for (auto statement : statements)
	{
		statement->Rollback(session);
	}
}