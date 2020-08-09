#ifndef GAMNET_DATABASE_MYSQL_TRANSACTION_H_
#define GAMNET_DATABASE_MYSQL_TRANSACTION_H_

#include "Connection.h"
#include <list>
#include "../../Library/String.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace MySQL {
class Transaction 
{
	struct UpdateQuery 
	{
		std::string table_name;
		std::string set_clause;
		std::map<std::string, Gamnet::Variant> where_clause;
	};

	struct InsertQuery 
	{
		std::string table_name;
		std::map<std::string, Gamnet::Variant> columns;
	};

	std::list<UpdateQuery> update_queries;
	std::list<InsertQuery> insert_queries;
	std::list<std::string> plain_queries;

	std::string queries;
	bool commit;
	std::shared_ptr<Connection> connection;
public:
	Transaction(int db_type);
	virtual ~Transaction();

	void Insert(const std::string& tableName, const std::map<std::string, Gamnet::Variant>& columns);
	void Update(const std::string& tableName, const std::string& columns, const std::map<std::string, Gamnet::Variant>& keys);
	template <class... ARGS>
	void Execute(ARGS... args)
	{
		if(true == commit)
		{
			throw Exception((int)ErrorCode::AlreadyCommitTransaction, "already commited transaction");
		}
		//queries += Format(args...) + ";";
		plain_queries.push_back(Format(args...));
	}
	ResultSet Commit();
};

} } }
#endif /* DATABASE_TRANSACTION_H_ */
