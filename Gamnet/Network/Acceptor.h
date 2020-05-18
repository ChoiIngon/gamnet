#ifndef GAMNET_NETWORK_ACCEPTOR_H_
#define GAMNET_NETWORK_ACCEPTOR_H_

#include <boost/asio.hpp>
#include <atomic>
#include "../Library/Pool.h"

namespace Gamnet { namespace Network {
	class SessionManager;
	class Acceptor
	{
	private :
		struct SocketFactory
		{
			SessionManager*	const	session_manager;
			SocketFactory(SessionManager* const manager);
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

		boost::asio::ip::tcp::acceptor	acceptor;
		boost::asio::ip::tcp::endpoint	endpoint;
		Pool<boost::asio::ip::tcp::socket, std::mutex, SocketInitFunctor, SocketReleaseFunctor> socket_pool;

		SessionManager*	const	session_manager;
		int						max_queue_size;
		std::atomic<int>		cur_queue_size;
	public:
		Acceptor(SessionManager* const manager);

		void Listen(int port, int max_count, int queue_size);
		void Release();
	private:
		void Accept();
		void Callback_Accept(const std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec);
	};
}}
#endif
