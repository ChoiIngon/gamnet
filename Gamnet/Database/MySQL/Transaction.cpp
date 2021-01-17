#include "Transaction.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace MySQL {

	Transaction::Query::Type Transaction::PlainQuery::GetType() const
	{
		return Transaction::Query::Type::Plain;
	}

	std::string Transaction::PlainQuery::MakeQuery() 
	{
		return query;
	}

	void Transaction::PlainQuery::Clear()
	{
		query = "";
	}

	Transaction::Query::Type Transaction::UpdateQuery::GetType() const
	{
		return Transaction::Query::Type::Update;
	}

	bool Transaction::UpdateQuery::CompareKey(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& whereClause)
	{
		if(this->table_name != tableName)
		{
			return false;
		}

		if(this->where_clause.size() != whereClause.size())
		{
			return false;
		}

		for(auto& itr : this->where_clause)
		{
			auto tmp = whereClause.find(itr.first);
			if(whereClause.end() == tmp)
			{
				return false;
			}

			if((std::string)tmp->second != (std::string)itr.second)
			{
				return false;
			}
		}

		return true;
	}

	std::string Transaction::UpdateQuery::MakeQuery()
	{
		std::string whereClause;
		for(const auto& itr : where_clause)
		{
			if("" != whereClause)
			{
				whereClause += " AND ";
			}

			whereClause += itr.first + "='" + (std::string)itr.second + "'";
		}
		
		return "UPDATE " + table_name + " SET " + set_clause + " WHERE " + whereClause;
	}

	void Transaction::UpdateQuery::Clear()
	{
		table_name = "";
		set_clause = "";
	}

	Transaction::Query::Type Transaction::InsertQuery::GetType() const
	{
		return Transaction::Query::Type::Insert;
	}
		
	bool Transaction::InsertQuery::CompareKey(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& columns)
	{
		if(this->table_name != tableName)
		{
			return false;
		}

		if(this->columns.size() != columns.size())
		{
			return false;
		}

		for(const std::string& column : this->columns)
		{
			if(columns.end() == columns.find(column))
			{
				return false;
			}
		}
		return true;
	}

	void Transaction::InsertQuery::AddColumns(const std::map<std::string, Gamnet::Variant>& columns)
	{
		for(const auto& itr : columns)
		{
			this->columns.insert(itr.first);
		}
	}

	void Transaction::InsertQuery::AddValues(const std::map<std::string, Gamnet::Variant>& columns)
	{
		std::string value = "";
		for(const auto& itr : columns)
		{
			if("" != value)
			{
				value += ",";
			}
			value += "'" + (std::string)itr.second + "'";
		}
		values.push_back("(" + value + ")");
	}

	std::string Transaction::InsertQuery::MakeQuery()
	{
		std::string query;
		std::string columnsClause;
		for(const auto& column : columns)
		{
			if("" != columnsClause)
			{
				columnsClause += ",";
			}
			columnsClause += "`" + column + "`";
		}
		std::string valuesClause;
		for(const auto& value : values)
		{
			if("" != valuesClause)
			{
				valuesClause += ",";
			}
			valuesClause += value;
		}
		return Format("INSERT INTO ", table_name, "(", columnsClause, ") VALUES ", valuesClause);
	}

	void Transaction::InsertQuery::Clear()
	{
		table_name = "";
		values.clear();
	}

Transaction::Transaction(int db_type) : db_type(db_type)
{
}

Transaction::~Transaction()
{
}

void Transaction::Insert(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& columns)
{
	if(0 <queries.size())
	{
		std::shared_ptr<Query> query = queries.back();
		if(Transaction::Query::Type::Insert == query->GetType())
		{
			std::shared_ptr<InsertQuery> insert = std::static_pointer_cast<InsertQuery>(query);
			if(true == insert->CompareKey(tableName, columns))
			{
				insert->AddValues(columns);
				return;
			}
		}
	}
	std::shared_ptr<InsertQuery> insert = std::make_shared<InsertQuery>();
	insert->table_name = tableName;
	insert->AddColumns(columns);
	insert->AddValues(columns);
	queries.push_back(insert);
}

void Transaction::Update(const std::string& tableName, const std::string& setClause, const std::map<std::string, Gamnet::Variant>& whereClause)
{
	if(0 < queries.size())
	{
		std::shared_ptr<Query> query = queries.back();
		if(Transaction::Query::Type::Update == query->GetType())
		{
			std::shared_ptr<UpdateQuery> update = std::static_pointer_cast<UpdateQuery>(query);
			if(true == update->CompareKey(tableName, whereClause))
			{
				update->set_clause += "," + setClause;
				return;
			}
		}
	}
	std::shared_ptr<UpdateQuery> update = std::make_shared<UpdateQuery>();
	update->table_name = tableName;
	update->set_clause = setClause;
	update->where_clause = whereClause;
	queries.push_back(update);
}

ResultSet Transaction::Commit()
{
	ResultSet res(nullptr);
	if(0 == queries.size())
	{
		return res;
	}

	std::string execute;
	for(const auto& query : queries)
	{
		execute += query->MakeQuery() + ";";
	}
	
	std::shared_ptr<Connection> connection = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	
	if(1 == queries.size())
	{
		res = connection->Execute(execute);
	}
	else 
	{
		try {
			res = connection->Execute("start transaction;" + execute + " commit;");
			while (res.NextResult());
		}
		catch (const Gamnet::Exception& e)
		{
			connection->Execute("rollback");
			throw Exception(e.error_code(), e.what(), "\n\t", execute);
		}
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
	queries.clear();
}

}}}