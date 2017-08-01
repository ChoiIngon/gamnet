#ifndef GAMNET_DATABASE_MYSQL_MYSQL_H_
#define GAMNET_DATABASE_MYSQL_MYSQL_H_

#include "Connection.h"
#include "Transaction.h"

namespace Gamnet { namespace Database { namespace MySQL {
	void ReadXml(const char* xml_path);
	bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);
	ResultSet Execute(int db_type, const std::string& query);
	template <class... ARGS>
	ResultSet Execute(int db_type, ARGS... args)
	{
		return Execute(db_type, Format(args...));
	}
}}}

#endif /* DATABASE_H_ */
