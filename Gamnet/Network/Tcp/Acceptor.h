#ifndef GAMNET_NETWORK_ACCEPTOR_H_
#define GAMNET_NETWORK_ACCEPTOR_H_

#include <boost/asio.hpp>
#include <atomic>
#include "../../Library/Json/json.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Acceptor
	{
	private :
		struct Deleter
		{
			Acceptor& acceptor;
			Deleter(Acceptor& acceptor);
			void operator()(void const* ptr) const;
		};

		boost::asio::ip::tcp::acceptor	acceptor;
		boost::asio::ip::tcp::endpoint	endpoint;
		int max_socket_count;

		std::atomic<int> accept_count;
		std::atomic<int> close_count;
		std::atomic<int> fail_count;
	public:
		Acceptor();

		std::function<void(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)> accept_handler;

		void Listen(int port, int max_count);
		Json::Value State();
	private:
		void Accept(boost::asio::ip::tcp::socket* socket);
		void Callback_Accept(const std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec);
	};
}}}
#endif
