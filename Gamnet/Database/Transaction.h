/*
 * Transaction.h
 *
 *  Created on: Jul 24, 2015
 *      Author: wered
 */

#ifndef DATABASE_TRANSACTION_H_
#define DATABASE_TRANSACTION_H_

#include "Connection.h"

namespace Gamnet {
namespace Database {

class Transaction {
	bool commit;
	std::shared_ptr<Connection> connection;
public:
	Transaction(int db_type);
	//Transaction(int db_type, std::function<void(Transaction& transaction)> t);
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
	void Commit();
};

} /* namespace Database */
} /* namespace Gamnet */

#endif /* DATABASE_TRANSACTION_H_ */
