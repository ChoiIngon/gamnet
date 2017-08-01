#ifndef GAMNET_DATABASE_REDIS_REDIS_H_
#define GAMNET_DATABASE_REDIS_REDIS_H_

#include "Connection.h"

namespace Gamnet { namespace Database { namespace Redis {
		void ReadXml(const char* xml_path);
		bool Connect(int db_type, const char* host, int port);
		ResultSet Execute(int db_type, const std::string& query);
		template <class... ARGS>
		ResultSet Execute(int db_type, ARGS... args)
		{
			return Execute(db_type, Format(args...));
		}
}}}

#endif /* DATABASE_H_ */

