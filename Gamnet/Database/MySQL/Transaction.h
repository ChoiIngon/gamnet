#ifndef GAMNET_DATABASE_MYSQL_TRANSACTION_H_
#define GAMNET_DATABASE_MYSQL_TRANSACTION_H_

#include "Connection.h"
#include <list>
#include "../../Library/String.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace MySQL {
class Transaction {
	std::string queries;
	bool commit;
	std::shared_ptr<Connection> connection;
public:
	Transaction(int db_type);
	virtual ~Transaction();

	template <class... ARGS>
	void Execute(ARGS... args)
	{
		if(true == commit)
		{
			throw Exception((int)ErrorCode::AlreadyCommitTransaction, "already commited transaction");
		}
		queries += Format(args...) + ";";
	}
	ResultSet Commit();
};

} } }
#endif /* DATABASE_TRANSACTION_H_ */
