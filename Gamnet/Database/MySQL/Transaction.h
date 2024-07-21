#ifndef GAMNET_DATABASE_MYSQL_TRANSACTION_H_
#define GAMNET_DATABASE_MYSQL_TRANSACTION_H_

#include "Connection.h"
#include <list>
#include <set>

namespace Gamnet { namespace Database { namespace MySQL {
class Transaction 
{
public :
	struct Query
	{
		enum class Type
		{
			Plain,
			Update,
			Insert
		};

		virtual Type GetType() const = 0;
		virtual std::string MakeQuery() = 0;
		virtual void Clear() = 0;
	};
private:
	struct PlainQuery : public Query
	{
		virtual Type GetType() const override;
		virtual std::string MakeQuery() override;
		virtual void Clear() override;

		std::string query;
	};

	struct UpdateQuery : public Query
	{
		virtual Type GetType() const override;
		virtual std::string MakeQuery() override;
		virtual void Clear() override;

		bool CompareKey(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& whereClause);
		std::string table_name;
		std::string set_clause;
		std::map<std::string, Gamnet::Variant> where_clause;
	};

	struct InsertQuery : public Query
	{
		virtual Type GetType() const override;
		virtual std::string MakeQuery() override;
		virtual void Clear() override;

		bool CompareKey(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& columns);
		void AddColumns(const std::map<std::string, Gamnet::Variant>& columns);
		void AddValues(const std::map<std::string, Gamnet::Variant>& columns);
		std::string table_name;
		std::set<std::string> columns;
		std::list<std::string> values;
	};

	int db_type;
	std::list<std::shared_ptr<Query>> queries;
public:
	Transaction(int db_type);
	virtual ~Transaction();

	void Insert(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& columns);
	void Update(const std::string& tableName, const std::string& columns, const std::map<std::string, Gamnet::Variant>& keys);
	template <class... ARGS>
	void Execute(ARGS... args)
	{
		/*
		plain_queries.push_back(Format(args...));
		query_count++;
		*/
		std::shared_ptr<PlainQuery> plain = std::make_shared<PlainQuery>();
		plain->query = Format(args...);
		queries.push_back(plain);
	}
	
	void Execute(const std::shared_ptr<Query>& query)
	{
		queries.push_back(query);
	}

	ResultSet Commit();
	void Rollback();
private :
	void Clear();
};

} } }
#endif /* DATABASE_TRANSACTION_H_ */
