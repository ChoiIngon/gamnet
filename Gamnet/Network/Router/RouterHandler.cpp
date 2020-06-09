#include "RouterHandler.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Router {

void RouterHandler::Recv_SetAddress_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Req req;
	MsgRouter_SetAddress_Ans ans;
	ans.error_code = ErrorCode::Success;

	try {
		if(false == Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
		}
		//LOG(INF, "[Gamnet::Router] ", session->socket->remote_endpoint().address().to_v4().to_string(), "->localhost RECV MsgRouter_SetAddress_Req(router_address:", req.router_address.ToString(), ")");
		SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
		ans.router_address = sessionManager->local_address;

		session->send_session = true;
		if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress(req.router_address, session))
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress);
		}
		sessionManager->Connect(req.host, req.port, 5);
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	//LOG(INF, "[Gamnet::Router] localhost->", session->socket->remote_endpoint().address().to_v4().to_string(), " SEND MsgRouter_SetAddress_Ans(error_code:", (int)ans.error_code, ", router_address:", ans.router_address.ToString(), ")");
	Network::Tcp::SendMsg(session, ans, false);
}

void RouterHandler::Recv_SetAddress_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Ans ans;
	if(false == Network::Tcp::Packet::Load(ans, packet))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
	}

	//LOG(INF, "[Gamnet::Router] ", session->socket->remote_endpoint().address().to_v4().to_string(), "->localhost RECV MsgRouter_SetAddress_Ans(error_code:", (int)ans.error_code, ", router_address:", ans.router_address.ToString(), ")");
	if(ErrorCode::Success != ans.error_code)
	{
		throw GAMNET_EXCEPTION(ErrorCode::SetRouterAddressError, "error_code:", ans.error_code);
	}
	session->router_address = ans.router_address;
	session->OnConnect();
}

void RouterHandler::Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SendMsg_Ntf ntf;
	
	if (false == Network::Tcp::Packet::Load(ntf, packet))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
	}

	Address addr = session->router_address;
	addr.msg_seq = ntf.msg_seq;
	std::shared_ptr<Network::Tcp::Packet> session_packet = Network::Tcp::Packet::Create();
	if (nullptr == session_packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
	}

	session_packet->Append(ntf.buffer.c_str(), ntf.buffer.length());
	session_packet->ReadHeader();
	Singleton<Dispatcher>::GetInstance().OnReceive(addr, session_packet);
}

void RouterHandler::Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	LOG(DEV, "[Router] recv heartbeat message(address:", session->router_address.ToString(),")");
}

}}}
