#ifndef _GAMNET_DATABASE_REDIS_CONNECTION_H_
#define _GAMNET_DATABASE_REDIS_CONNECTION_H_

#include <boost/asio.hpp>
#include <list>
#include "ResultSet.h"
#include "../../Library/Json/json.h"
#include "../../Library/Timer.h"

namespace Gamnet { namespace Database {	namespace Redis {
	class Connection : public std::enable_shared_from_this<Connection> 
	{
	public:
		struct ConnectionInfo
		{
			std::string host;
			int port;
		};

		bool Reconnect();
	protected :
		boost::asio::ip::tcp::socket socket_;
	public:
		ConnectionInfo connection_info;
		boost::asio::deadline_timer deadline_;

		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
	private :
		int Send(const std::string& query);
	};
} } }
#endif
