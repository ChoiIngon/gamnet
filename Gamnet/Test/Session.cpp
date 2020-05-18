#include "Session.h"
#include "../Library/Timer.h"
#include "../Network/Tcp/SystemMessageHandler.h"

namespace Gamnet { namespace Test {

Session::Session() : 
	server_session_key(0),
	server_session_token(""),
	test_seq(-1), 
	is_pause(false),
	is_connected(false)
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
	server_session_key = 0;
	server_session_token = "";
	test_seq = 0;
	is_pause = false;
	is_connected = false;

	return true;
}

void Session::Clear()
{
	Network::Tcp::Session::Clear();
}

void Session::Pause(int millisecond)
{
	is_pause = true;
	std::shared_ptr<Time::Timer> timer = Time::Timer::Create();
	std::shared_ptr<Session> self = std::static_pointer_cast<Session>(shared_from_this());
	timer->SetTimer(millisecond, strand->wrap([self, timer]() {
		self->execute_send_handler(self);
	})); 
}

void Session::Resume()
{
	is_pause = false;
}

void Session::Next()
{
	test_seq++;
	//LOG(DEV, "session_key:", session_key, ", link_key:", link->link_key, ", test_seq:", test_seq);
	is_pause = false;
	while (0 < send_packets.size())
	{
		send_packets.pop_front();
	}
}

void Session::OnConnectHandler()
{
	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if (nullptr == packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::CreateInstanceFailError, "can not create packet");
	}

	if("" == session_token)
	{
		packet->Write(Network::Tcp::MsgID_CliSvr_Connect_Req, nullptr, 0);
	}
	else
	{
		Json::Value req;
		req["session_key"] = server_session_key;
		req["session_token"] = session_token;

		Json::FastWriter writer;
		std::string str = writer.write(req);

		packet->Write(Network::Tcp::MsgID_CliSvr_Reconnect_Req, str.c_str(), str.length());
	}

	AsyncSend(packet);
}
}}/* namespace Gamnet */
