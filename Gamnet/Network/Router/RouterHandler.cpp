#include "RouterHandler.h"
#include "../../Log/Log.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "LinkManager.h"
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
		LOG(INF, "[Router-", session->link->link_key, "-", session->session_key, "] ", session->GetRemoteAddress().to_string(), "->localhost RECV MsgRouter_SetAddress_Req(router_address:", req.router_address.ToString(), ")");
		ans.router_address = Singleton<LinkManager>::GetInstance().local_address;
		if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress(req.router_address, session))
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress);
		}
		Singleton<LinkManager>::GetInstance().Connect(req.host.c_str(), req.port, 5);
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Router-", session->link->link_key, "-", session->session_key, "] localhost->", session->GetRemoteAddress().to_string(), " SEND MsgRouter_SetAddress_Ans(error_code:", (int)ans.error_code, ", router_address:", ans.router_address.ToString(), ")");
	SendMsg(session, ans, false);
}
void RouterHandler::Recv_SetAddress_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Ans ans;
	
	if(false == Network::Tcp::Packet::Load(ans, packet))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
	}

	LOG(INF, "[Router-", session->link->link_key, "-", session->session_key, "] ", session->GetRemoteAddress().to_string(), "->localhost RECV MsgRouter_SetAddress_Ans(error_code:", (int)ans.error_code, ", router_address:", ans.router_address.ToString(), ")");
	if(ErrorCode::Success != ans.error_code)
	{
		throw GAMNET_EXCEPTION(ans.error_code);
	}

	session->address = ans.router_address;
	session->OnConnect();

	AtomicPtr<Tcp::CastGroup> lockedCastGroup(Singleton<LinkManager>::GetInstance().heartbeat_group);
	lockedCastGroup->AddSession(session);
}

void RouterHandler::Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SendMsg_Ntf ntf;
	
	if (false == Network::Tcp::Packet::Load(ntf, packet))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
	}

	Address addr = session->address;
	addr.msg_seq = ntf.msg_seq;
	std::shared_ptr<Network::Tcp::Packet> session_packet = Network::Tcp::Packet::Create();
	if (nullptr == session_packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
	}

	session_packet->Append(ntf.buffer.c_str(), ntf.buffer.length());
	session_packet->ReadHeader();
	Singleton<Dispatcher>::GetInstance().OnRecvMsg(addr, session_packet);
}

void RouterHandler::Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	LOG(DEV, "[Router] recv heartbeat message(address:", session->address.service_name, ":", (int)session->address.cast_type, ":", session->address.id,")");
}

}}}
