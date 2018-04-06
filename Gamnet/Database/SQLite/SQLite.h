#ifndef GAMNET_DATABASE_SQLITE_SQLITE_H_
#define GAMNET_DATABASE_SQLITE_SQLITE_H_

#include "Connection.h"
#include "Transaction.h"

namespace Gamnet { namespace Database { namespace SQLite {
	bool Connect(int db_type, const char* db);
	ResultSet Execute(int db_type, const std::string& query);
	template <class... ARGS>
	ResultSet Execute(int db_type, ARGS... args)
	{
		return Execute(db_type, Format(args...));
	}
}}}

#endif /* DATABASE_H_ */

