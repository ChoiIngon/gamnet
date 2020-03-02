#ifndef _GAMNET_DATABASE_REDIS_CONNECTION_H_
#define _GAMNET_DATABASE_REDIS_CONNECTION_H_

#include <boost/asio.hpp>
#include <list>
#include "ResultSet.h"
#include "../../Library/Json/json.h"
#include "../../Library/Timer.h"
#include "../../Network/Link.h"

namespace Gamnet { namespace Database {	namespace Redis {
	class Connection : public Network::Link
	{
	public:
		struct ConnectionInfo
		{
			std::string host;
			int port;
		};

		Connection();
		virtual ~Connection();

		ConnectionInfo connection_info;

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
	protected :
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override {}
	private :
		bool Reconnect();
	};
} } }
#endif
