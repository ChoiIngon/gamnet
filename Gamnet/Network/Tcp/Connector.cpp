#include "Connector.h"
#include "SessionManager.h"
#include "../../Library/Singleton.h"

namespace Gamnet { namespace Network { namespace Tcp {
	boost::asio::ip::tcp::socket* Connector::SocketFactory::operator() ()
	{
		return new boost::asio::ip::tcp::socket(Singleton<boost::asio::io_service>::GetInstance());
	}

	boost::asio::ip::tcp::socket* Connector::SocketInitFunctor::operator() (boost::asio::ip::tcp::socket* socket)
	{
		return socket;
	}

	boost::asio::ip::tcp::socket* Connector::SocketReleaseFunctor::operator() (boost::asio::ip::tcp::socket* socket)
	{
		socket->close();
		return socket;
	}
	
	Connector::Connector() : socket_pool(65535, SocketFactory())

	{
	}

	void Connector::AsyncConnect(const char* host, int port, int timeout)
	{
		if (nullptr == host)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "host is null");
		}

		if (0 == strlen(host))
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "host is empty");
		}

		boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>::GetInstance());
		boost::asio::ip::tcp::resolver::query query_(host, "");
		boost::asio::ip::address address_;
		for (auto itr = resolver_.resolve(query_); itr != boost::asio::ip::tcp::resolver::iterator(); ++itr)
		{
			boost::asio::ip::tcp::endpoint end = *itr;
			address_ = end.address();
			break;
		}

		boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ address_.to_v4().to_string(), std::to_string(port).c_str() }));

		std::shared_ptr<boost::asio::ip::tcp::socket> socket = socket_pool.Create();
		if (nullptr == socket)
		{
			LOG(GAMNET_ERR, "can not create socket. deny addtional connection");
			return;
		}
		
		std::shared_ptr<Time::Timer> timer = nullptr;
		if (0 < timeout)
		{
			timer = Time::Timer::Create();
			timer->AutoReset(false);
			timer->SetTimer(timeout * 1000, std::bind(&Connector::Callback_ConnectTimeout, this, socket, timer));
		}
		socket->async_connect(endpoint_, boost::bind(&Connector::Callback_Connect, this, socket, timer, endpoint_, boost::asio::placeholders::error));
	}

	bool Connector::SyncConnect(const char* host, int port, int timeout)
	{
		std::shared_ptr<boost::asio::ip::tcp::socket> socket = socket_pool.Create();
		if (nullptr == socket)
		{
			LOG(GAMNET_ERR, "can not create socket. deny addtional connection");
			return false;
		}

		std::shared_ptr<Time::Timer> timer = nullptr;
		if(timeout > 0)
		{
			timer = Time::Timer::Create();
			timer->AutoReset(false);
			timer->SetTimer(timeout * 1000, std::bind(&Connector::Callback_ConnectTimeout, this, socket, timer));
		}
		
		boost::asio::ip::address address_;
		boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>::GetInstance());
		boost::asio::ip::tcp::resolver::query query_(host, "");
		for (auto itr = resolver_.resolve(query_); itr != boost::asio::ip::tcp::resolver::iterator(); ++itr)
		{
			boost::asio::ip::tcp::endpoint end = *itr;
			address_ = end.address();
			break;
		}

		boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ address_.to_v4().to_string(), std::to_string(port).c_str() }));

		boost::system::error_code ec;
		socket->connect(endpoint_, ec);
		if (0 != ec)
		{
			LOG(GAMNET_ERR, "connect fail(host:", host, ", port:", port, ")");
			return false;
		}

		timer->Cancel();
		connect_handler(socket);
		return true;
	}

	void Connector::Callback_Connect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer, const boost::asio::ip::tcp::endpoint& endpoint, const boost::system::error_code& ec)
	{
		try {
			if(nullptr != timer)
			{
				timer->Cancel();
			}

			if (false == socket->is_open())
			{
				return;
			}
			else if (0 != ec)
			{
				throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "connect fail(dest:", endpoint.address().to_v4().to_string(), ", message:", ec.message(), ", errno:", ec, ")");
			}

			connect_handler(socket);
			return;
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), ", error_code:", e.error_code());
		}
		catch (const boost::system::system_error& e)
		{
			LOG(ERR, "connect fail(dest:", endpoint.address().to_v4().to_string(), ", errno:", e.code().value(), ", errstr:", e.what(), ")");
		}
	}

	void Connector::Callback_ConnectTimeout(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer)
	{
		if(nullptr == timer)
		{
			return;
		}
		timer->Cancel();
	}

}}}