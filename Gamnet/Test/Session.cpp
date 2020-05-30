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
	if(false == Network::Session::Init())
	{
		return false;
	}
	test_seq = 0;
	session_token = "";
	recv_packet = Network::Tcp::Packet::Create();
	if (nullptr == recv_packet)
	{
		LOG(GAMNET_ERR, "ErrorCode::NullPacketError can not create Packet instance");
		return false;
	}

	recv_seq = 0;
	send_seq = 0;
	handover_safe = false;
	return true;
}

void Session::Clear()
{
	send_packets.clear();
	Network::Session::Clear();
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
	auto self = std::static_pointer_cast<Session>(shared_from_this());
	timer->SetTimer(5000, strand->wrap(std::bind(&Session::Callback_ConnectTimeout, self, socket, timer)));
	socket->async_connect(endpoint_, strand->wrap(boost::bind(&Session::Callback_Connect, self, socket, timer, endpoint_, boost::asio::placeholders::error)));
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
		AsyncRead();
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if (nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
		}

		Json::Value req;
		req["session_key"] = session_key;
		req["session_token"] = session_token;

		Json::FastWriter writer;
		std::string str = writer.write(req);

		packet->Write(Network::Tcp::MsgID_CliSvr_Reconnect_Req, str.c_str(), str.length());

		send_buffers.push_front(packet);
		FlushSend();
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
