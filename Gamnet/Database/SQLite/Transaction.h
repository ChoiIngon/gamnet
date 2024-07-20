#ifndef GAMNET_DATABASE_SQLITE_TRANSACTION_H_
#define GAMNET_DATABASE_SQLITE_TRANSACTION_H_

#include "Connection.h"
import Gamnet.String;

namespace Gamnet { namespace Database { namespace SQLite {
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
}}}

#endif
