#include "Transaction.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace MySQL {

Transaction::Transaction(int db_type) : db_type(db_type), query_count(0)
{
}

Transaction::~Transaction()
{
}

void Transaction::Insert(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& columns)
{
	InsertQuery query;
	query.table_name = tableName;
	query.columns = columns;
	insert_queries.push_back(query);
	query_count++;
}

void Transaction::Update(const std::string& tableName, const std::string& setClause, const std::map<std::string, Gamnet::Variant>& whereClause)
{
	UpdateQuery query;
	query.table_name = tableName;
	query.set_clause = setClause;
	query.where_clause = whereClause;
	update_queries.push_back(query);
	query_count++;
}

ResultSet Transaction::Commit()
{
	ResultSet res;
	if(0 == query_count)
	{
		return res;
	}

	int queryCount = 0;
	std::string queries;
	{
		// key { table_name, column_clause } : values_clause
		std::map<std::pair<std::string, std::string>, std::string> inserts;
		for (const auto& query : insert_queries)
		{
			std::string columnsClause;
			std::string valuesClause;
			for (const auto& itr : query.columns)
			{
				if ("" != columnsClause)
				{
					columnsClause += ",";
				}
				if ("" != valuesClause)
				{
					valuesClause += ",";
				}

				columnsClause += "`" + itr.first + "`";
				valuesClause += "'" + (std::string)itr.second + "'";
			}

			auto key = std::make_pair(query.table_name, columnsClause);

			std::string& insertQuery = inserts[key];
			if ("" != insertQuery)
			{
				insertQuery += ",";
			}
			insertQuery += "(" + valuesClause + ")";
		}

		for (const auto& itr : inserts)
		{
			queryCount++;
			queries += Format("insert into ", itr.first.first, "(", itr.first.second, ") values ", itr.second, ";");
		}
	}
	{
		// key { table_name, where_clause } : set_clause
		std::map<std::pair<std::string, std::string>, std::string> updates;
		for (const auto& query : update_queries)
		{
			std::string whereClause;
			for (const auto& itr : query.where_clause)
			{
				if ("" != whereClause)
				{
					whereClause += " and ";
				}

				whereClause += itr.first + "='" + (std::string)itr.second + "'";
			}

			auto key = std::make_pair(query.table_name, whereClause);

			std::string& updateQuery = updates[key];
			if ("" != updateQuery)
			{
				updateQuery += ",";
			}
			updateQuery += query.set_clause;
		}

		for (const auto& itr : updates)
		{
			queryCount++;
			queries += Format("update ", itr.first.first, " set ", itr.second, " where ", itr.first.second, ";");
		}
	}
	{
		for(const auto& query : plain_queries)
		{
			queryCount++;
			queries += query + ";";
		}
	}
	
	std::shared_ptr<Connection> connection = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	try {
		if(1 == queryCount)
		{
			res.impl_ = connection->Execute(queries);
		}
		else 
		{
			res.impl_ = connection->Execute("start transaction;" + queries + " commit;");
		}
		while (res.NextResult());
	}
	catch (const Gamnet::Exception& e)
	{
		connection->Execute("rollback");
		// CR_CONN_HOST_ERROR
		if(nullptr != connection->logger_)
		{
			connection->logger_->Write(Log::Logger::LOG_LEVEL_INF, "start transaction; " + queries + " commit;");
		}
		throw e;
	}
	
	Clear();
	return res;
}

void Transaction::Rollback()
{
	Clear();
}

void Transaction::Clear()
{
	query_count = 0;
	update_queries.clear();
	insert_queries.clear();
	plain_queries.clear();
}

} /* namespace Database */
} /* namespace Gamnet */
}