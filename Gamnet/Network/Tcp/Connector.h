#ifndef GAMNET_NETWORK_CONNECTOR_H_
#define GAMNET_NETWORK_CONNECTOR_H_

#include <boost/asio.hpp>
#include <atomic>
#include "../../Library/Pool.h"
#include "../../Library/Timer.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Connector
	{
	private:
		struct SocketFactory
		{
			boost::asio::ip::tcp::socket* operator() ();
		};

		struct SocketInitFunctor
		{
			boost::asio::ip::tcp::socket* operator() (boost::asio::ip::tcp::socket* socket);
		};

		struct SocketReleaseFunctor
		{
			boost::asio::ip::tcp::socket* operator() (boost::asio::ip::tcp::socket* socket);
		};

		Pool<boost::asio::ip::tcp::socket, std::mutex, SocketInitFunctor, SocketReleaseFunctor> socket_pool;
	public:
		Connector();

		std::function<void(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)> connect_handler;
		void AsyncConnect(const char* host, int port, int timeout);
		bool SyncConnect(const char* host, int port, int timeout);
	private:
		void Callback_Connect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer, const boost::asio::ip::tcp::endpoint& endpoint, const boost::system::error_code& ec);
		void Callback_ConnectTimeout(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer);
	};
}}}
#endif
