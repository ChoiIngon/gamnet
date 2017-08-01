#ifndef _GAMNET_DATABASE_REDIS_CONNECTION_H_
#define _GAMNET_DATABASE_REDIS_CONNECTION_H_

#include <boost/asio.hpp>
#include <list>
#include "ResultSet.h"

namespace Gamnet { namespace Database {	namespace Redis {
	class Connection {
		boost::asio::ip::tcp::socket socket_;
		void Parse(const std::shared_ptr<ResultSetImpl>& impl, std::list<std::string>::iterator& itr_token);
	public:
		struct ConnectionInfo
		{
			std::string host;
			int port;
		};

		boost::asio::deadline_timer deadline_;

		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
	};
} } }
#endif
