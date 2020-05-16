#include "Acceptor.h"
#include <boost/bind.hpp>
#include "../Library/Singleton.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network {
	static boost::asio::io_service& io_service = Singleton<boost::asio::io_service>::GetInstance();

	Acceptor::SocketFactory::SocketFactory(SessionManager* const manager) : session_manager(manager)
	{
	}

	boost::asio::ip::tcp::socket* Acceptor::SocketFactory::operator() ()
	{
		return new boost::asio::ip::tcp::socket(io_service);
	}

	boost::asio::ip::tcp::socket* Acceptor::SocketInitFunctor::operator() (boost::asio::ip::tcp::socket* socket)
	{
		return socket;
	}

	Acceptor::SocketReleaseFunctor::SocketReleaseFunctor(Acceptor& acceptor) : acceptor(acceptor)
	{
	}

	boost::asio::ip::tcp::socket* Acceptor::SocketReleaseFunctor::operator() (boost::asio::ip::tcp::socket* socket)
	{
		acceptor.Release();
		socket->close();
		return socket;
	}

	Acceptor::Acceptor(SessionManager* const manager) :
		acceptor(io_service),
		socket_pool(65535, SocketFactory(manager), SocketInitFunctor(), SocketReleaseFunctor(*this)),
		session_manager(manager),
		max_queue_size(0),
		cur_queue_size(0)
	{
	}

	void Acceptor::Listen(int port, int max_count, int queue_size)
	{
		socket_pool.Capacity(max_count);
		endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor.open(endpoint.protocol());
		acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor.bind(endpoint);
		acceptor.listen();

		max_queue_size = queue_size;
		for (int i = 0; i < this->max_queue_size; i++)
		{
			cur_queue_size++;
			Accept();
		}
	}

	void Acceptor::Release()
	{
		if (cur_queue_size < max_queue_size)
		{
			cur_queue_size++;
			Accept();
		}
	}

	void Acceptor::Accept()
	{
		std::shared_ptr<boost::asio::ip::tcp::socket> socket = socket_pool.Create();
		if (nullptr == socket)
		{
			LOG(GAMNET_ERR, "can not create socket. deny addtional connection");
			cur_queue_size = std::max(cur_queue_size - 1, 0);
			return;
		}

		acceptor.async_accept(*socket, boost::bind(&Acceptor::Callback_Accept, this, socket, boost::asio::placeholders::error));
	}

	void Acceptor::Callback_Accept(const std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec)
	{
		Accept();

		try {
			if (0 != ec)
			{
				throw GAMNET_EXCEPTION(ErrorCode::AcceptFailError, "error_code:", ec.value());
			}

			session_manager->OnAccept(socket);
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
		socket->close();
	}

}}