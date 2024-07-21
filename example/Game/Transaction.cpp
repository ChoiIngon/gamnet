#include "Transaction.h"

Transaction::Transaction(int index)
    :db_index(index)
{
}

Transaction::~Transaction()
{
	Clear();
}

void Transaction::Execute(const std::shared_ptr<Query>& query)
{
    queries.push_back(query);
}

Transaction::ResultSet Transaction::Commit()
{
	ResultSet res(nullptr);
	if (0 == queries.size())
	{
		return res;
	}

	std::string execute;
	for (const auto& query : queries)
	{
		execute += query->Commit() + ";";
	}

	namespace DB = Gamnet::Database::MySQL;

	if (1 == queries.size())
	{
		res = DB::Execute(db_index, execute);
	}
	else
	{
		try {
			res = DB::Execute(db_index, "start transaction;" + execute + " commit;");
			while (res.NextResult());
		}
		catch (const Gamnet::Exception& e)
		{
			res = DB::Execute(db_index, "rollback");
			Rollback();
			throw Gamnet::Exception(e.error_code(), e.what(), "\n\t", execute);
		}
	}

	Clear();
	return res;
}

void Transaction::Rollback()
{
	for (const auto& query : queries)
	{
		query->Rollback();
	}
}

void Transaction::Clear()
{
	queries.clear();
}
