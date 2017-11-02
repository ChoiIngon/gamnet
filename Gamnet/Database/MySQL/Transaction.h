#ifndef GAMNET_DATABASE_MYSQL_TRANSACTION_H_
#define GAMNET_DATABASE_MYSQL_TRANSACTION_H_

#include "Connection.h"
#include "../../Library/String.h"

namespace Gamnet { namespace Database { namespace MySQL {
class Transaction {
	bool commit;
	std::shared_ptr<Connection> connection;
public:
	Transaction(int db_type);
	virtual ~Transaction();

	template <class... ARGS>
	ResultSet Execute(ARGS... args)
	{
		const std::string query = Format(args...);
		ResultSet res;
		res.impl_ = connection->Execute(query);
		res.impl_->conn_ = connection;
		return res;
	}
	ResultSet Commit();
};

} } }
#endif /* DATABASE_TRANSACTION_H_ */
