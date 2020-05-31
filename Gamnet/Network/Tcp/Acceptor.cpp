#include "Acceptor.h"
#include "SessionManager.h"
#include "../../Library/Singleton.h"

#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Tcp {
	
	Acceptor::Deleter::Deleter(Acceptor& acceptor) : acceptor(acceptor)
	{
	}

	void Acceptor::Deleter::operator() (void const* ptr) const
	{
		boost::asio::ip::tcp::socket* socket = (boost::asio::ip::tcp::socket*)ptr;
		socket->close();
		acceptor.close_count++;
		acceptor.Accept(socket);
	}

	Acceptor::Acceptor() 
		: acceptor(Singleton<boost::asio::io_service>::GetInstance())
		, max_socket_count(0)
		, accept_count(0)
		, close_count(0)
		, fail_count(0)
	{
	}

	void Acceptor::Listen(int port, int max_count)
	{
		endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor.open(endpoint.protocol());
		acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor.bind(endpoint);
		acceptor.listen();
		max_socket_count = max_count;
		for (int i = 0; i < max_count; i++)
		{
			Accept(new boost::asio::ip::tcp::socket(Singleton<boost::asio::io_service>::GetInstance()));
		}
	}

	Json::Value Acceptor::State()
	{
		Json::Value root;
		root["name"] = "Gamnet::Network::Tcp::Acceptor";
		root["accept_count"] = (int)accept_count;
		root["close_count"] = (int)close_count;
		root["fail_count"] = (int)fail_count;
		root["max_socket_count"] = (int)max_socket_count;
		root["active_socket_count"] = (int)(accept_count - close_count);
		root["idle_socket_count"] = (int)(max_socket_count - (accept_count - close_count));
		return root;
	}

	void Acceptor::Accept(boost::asio::ip::tcp::socket* pSocket)
	{
		std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::shared_ptr<boost::asio::ip::tcp::socket>(pSocket, Deleter(*this));
		if (nullptr == socket)
		{
			fail_count++;
			LOG(GAMNET_ERR, "can not create socket. deny addtional connection");
			return;
		}

		acceptor.async_accept(*socket, boost::bind(&Acceptor::Callback_Accept, this, socket, boost::asio::placeholders::error));
	}

	void Acceptor::Callback_Accept(const std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec)
	{
		try {
			if (0 != ec)
			{
				fail_count++;
				throw GAMNET_EXCEPTION(ErrorCode::AcceptFailError, "error_code:", ec.value());
			}
			accept_count++;
			accept_handler(socket);
			return;
		}
		catch (const Exception& e)
		{
			LOG(GAMNET_ERR, "accept fail(errno:", e.error_code(), ")");
		}
		catch (const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "accept fail(errno:", e.code().value(), ", errstr:", e.what(), ")");
		}
	}
}}}