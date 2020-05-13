#ifndef GAMNET_NETWORK_ACCEPTOR_H_
#define GAMNET_NETWORK_ACCEPTOR_H_

#include "../Library/Pool.h"
#include "../Library/Debugs.h"
#include "../Library/Delegate.h"
#include <boost/asio.hpp>

namespace Gamnet { namespace Network {
	class SessionManager;
	class Acceptor
	{
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
			Acceptor& acceptor;
			SocketReleaseFunctor(Acceptor& acceptor);
			boost::asio::ip::tcp::socket* operator() (boost::asio::ip::tcp::socket* socket);
		};

	private:
		boost::asio::ip::tcp::acceptor	acceptor;
		boost::asio::ip::tcp::endpoint	endpoint;
		Pool<boost::asio::ip::tcp::socket, std::mutex, SocketInitFunctor, SocketReleaseFunctor> socket_pool;

		SessionManager*	const			session_manager;
		int								max_queue_size;
		std::atomic<int>				cur_queue_size;
	public:
		Acceptor(SessionManager* manager);

		void Listen(int port, int max_count, int queue_size);
		void Release();
	private:
		void Accept();
		void Callback_Accept(const std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec);
	};
}}
#endif
