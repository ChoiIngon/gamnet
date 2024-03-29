#include "Session.h"
#include "../Library/Time/Time.h"
#include "../Network/Tcp/SystemMessageHandler.h"

namespace Gamnet { namespace Test {

Session::Session() :  test_seq(0)
{
#ifdef _DEBUG
	reconnector.name = "Test::Session";
#endif
	reconnector.connect_handler = std::bind(&Session::OnReconnect, this, std::placeholders::_1);
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
	auto self = std::static_pointer_cast<Session>(shared_from_this());
	Dispatch([this, self]() {
		if (nullptr == socket && true == handover_safe)
		{
			reconnector.AsyncConnect(host, port, 0);
		}
	});
	Network::Tcp::Session::AsyncSend(packet);
}

void Session::OnReconnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	auto self = shared_from_this();
	Dispatch([this, self, socket]() {
		boost::asio::socket_base::linger linger( true, 0 );
		socket->set_option( linger );

		boost::asio::socket_base::send_buffer_size send_buffer_size( Buffer::MAX_SIZE );
		socket->set_option( send_buffer_size );

		this->socket = socket;
		this->AsyncRead();
		this->Send_Reconnect_Req();
	});
}

void Session::Send_Connect_Req()
{
	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if (nullptr == packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::CreateInstanceFailError, "can not create packet");
	}
	packet->Write(Network::Tcp::MsgID_CliSvr_Connect_Req, nullptr, 0);
	Network::Session::AsyncSend(packet);
}

void Session::Recv_Connect_Ans(const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	std::string json = std::string(packet->ReadPtr() + Network::Tcp::Packet::HEADER_SIZE, packet->Size());
	Json::Value ans;
	Json::Reader reader;
	if (false == reader.parse(json, ans))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[Gamnet::Test] parse error(msg:", json, ")");
	}

	if (ErrorCode::Success != ans["error_code"].asInt())
	{
		throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[Gamnet::Test] connect fail(error_code:", ans["error_code"].asInt(), ")");
	}

	session_key = ans["session_key"].asUInt();
	session_token = ans["session_token"].asString();
	handover_safe = true;
	OnConnect();
}

void Session::Send_Reconnect_Req()
{
	//LOG(DEV, "[", link->link_manager->name, "/", link->link_key, "/", session->session_key, "] Send_Reconnect_Req");

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
	send_buffers.clear();
	Network::Session::AsyncSend(packet);
}

void Session::Recv_Reconnect_Ans(const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	//LOG(DEV, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Recv_Reconnect_Ans");
	std::string json = std::string(packet->ReadPtr() + Network::Tcp::Packet::HEADER_SIZE, packet->Size());
	Json::Value ans;
	Json::Reader reader;
	if (false == reader.parse(json, ans))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "parse error(msg:", json, ")");
	}

	if (ErrorCode::Success != ans["error_code"].asInt())
	{
		throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[Gamnet::Test] reconnect fail(error_code:", ans["error_code"].asInt(), ")");
	}

	OnConnect();
	for(std::shared_ptr<Network::Tcp::Packet> sentPacket : send_packets)
	{
		Network::Session::AsyncSend(sentPacket);
	}
}

void Session::Send_ReliableAck_Ntf()
{
	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if (nullptr == packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
	}

	Json::Value ntf;
	ntf["ack_seq"] = recv_seq;

	Json::FastWriter writer;
	std::string str = writer.write(ntf);

	packet->Write(Network::Tcp::MsgID_CliSvr_ReliableAck_Ntf, str.c_str(), str.length());
	Network::Session::AsyncSend(packet);
	//LOG(DEV, "[link_key:", link->link_key, "]");
}

void Session::Send_Close_Req()
{
	if(false == handover_safe)
	{
		return;
	}

	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if (nullptr == packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
	}
	if (false == packet->Write(Network::Tcp::MsgID_CliSvr_Close_Req, nullptr, 0))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize packet");
	}

	//LOG(DEV, "[", session->link->link_manager->name, "::link_key:", session->link->link_key, "]");
	handover_safe = false;
	Network::Session::AsyncSend(packet);
}

void Session::Recv_Close_Ans(const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	Close(ErrorCode::Success);
}

void Session::Close(int reason)
{
	Network::Session::Close(reason);
}
}}/* namespace Gamnet */
