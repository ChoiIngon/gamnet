#include "Session.h"

#include "../Network/Tcp/SystemMessageHandler.h"

namespace Gamnet { namespace Test {

Session::Session() :  test_seq(0)
{
}

Session::~Session() {
}

bool Session::Init()
{
	if(false == Network::Tcp::Session::Init())
	{
		return false;
	}
	test_seq = 0;
	return true;
}

void Session::Clear()
{
	Network::Tcp::Session::Clear();
}

void Session::Next()
{
	test_seq++;
	while(0 < send_packets.size())
	{
		send_packets.pop_front();
	}
}

void Session::AsyncSend(const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	if(nullptr == socket)
	{
		AsyncConnect();
	}
	Network::Tcp::Session::AsyncSend(packet);
}

void Session::AsyncConnect()
{
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
	std::shared_ptr<boost::asio::ip::tcp::socket> socket = std::make_shared<boost::asio::ip::tcp::socket>(Singleton<boost::asio::io_service>::GetInstance());
	if (nullptr == socket)
	{
		return;
	}

	std::shared_ptr<Time::Timer> timer = Time::Timer::Create();
	timer->AutoReset(false);
	timer->SetTimer(5000, strand->wrap(std::bind(&Session::Callback_ConnectTimeout, this, socket, timer)));

	socket->async_connect(endpoint_, strand->wrap(boost::bind(&Session::Callback_Connect, this, socket, timer, endpoint_, boost::asio::placeholders::error)));
}

void Session::Callback_Connect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer, const boost::asio::ip::tcp::endpoint& endpoint, const boost::system::error_code& ec)
{
	try {
		if (nullptr != timer)
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
		
		this->socket = socket;
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

void Session::Callback_ConnectTimeout(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer)
{
	if (nullptr == timer)
	{
		return;
	}
	timer->Cancel();
}
}}/* namespace Gamnet */
