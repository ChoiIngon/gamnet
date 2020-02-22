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
		LOG(INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", req.router_address.service_name, "] ", session->GetRemoteAddress().to_string(), "->localhost recv MsgRouter_SetAddress_Req(", req.router_address.service_name, ":", ToString<ROUTER_CAST_TYPE>(req.router_address.cast_type), ":", req.router_address.id, ")");
		LOG(INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", req.router_address.service_name, "] set remote router address(", req.router_address.service_name, ":", ToString<ROUTER_CAST_TYPE>(req.router_address.cast_type), ":", req.router_address.id, ")");
		session->address = req.router_address;
		ans.router_address = Singleton<LinkManager>::GetInstance().local_address;
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", req.router_address.service_name, "] localhost->", session->GetRemoteAddress().to_string(), " send MsgRouter_SetAddress_Ans(", ans.router_address.service_name, ":", ToString<ROUTER_CAST_TYPE>(ans.router_address.cast_type), ":", ans.router_address.id, ")");
	SendMsg(session, ans, false);
}
void RouterHandler::Recv_SetAddress_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Ans ans;
	try {
		if(false == Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
		}

		if(ErrorCode::Success != ans.error_code)
		{
			//std::shared_ptr<Link> _link = session->link;
			//_link->Close(ans.error_code);
			LOG(ERR, "[Router] [", session->link->link_key, "/", session->session_key, "/", ans.router_address.service_name, "] MsgRouter_SetAddress_Ans(error_code:", ans.error_code, ")");
			return;
		}

		LOG(GAMNET_INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", ans.router_address.service_name, "] ", session->GetRemoteAddress().to_string(), "->localhost recv MsgRouter_SetAddress_Ans(", ans.router_address.service_name, ":", ToString<ROUTER_CAST_TYPE>(ans.router_address.cast_type), ":", ans.router_address.id, ")");
		session->address = ans.router_address;
		if(false == Singleton<RouterCaster>::GetInstance().RegisterAddress(ans.router_address, session))
		{
			LOG(ERR, "[Router] [", session->link->link_key, "/", session->session_key, "/", ans.router_address.service_name, "] MsgRouter_SetAddress_Ans(error_code:", ans.error_code, ")");
			return;
		}
		
		MsgRouter_SetAddress_Ntf ntf;
		LOG(GAMNET_INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", ans.router_address.service_name, "] localhost->", session->GetRemoteAddress().to_string(), " send MsgRouter_SetAddress_Ntf()");
		SendMsg(session, ntf);
		session->onRouterConnect(session->address);
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code() ,")");
	}
}
void RouterHandler::Recv_SetAddress_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Ntf ntf;
	try {
		if(false == Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
		}
		LOG(GAMNET_INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", session->address.service_name, "] ", session->GetRemoteAddress().to_string(), "->localhost recv MsgRouter_SetAddress_Ntf()");
		if(false == Singleton<RouterCaster>::GetInstance().RegisterAddress(session->address, session))
		{
			LOG(GAMNET_INF, "[Router] [", session->link->link_key, "/", session->session_key, "/", session->address.service_name, "] ", session->GetRemoteAddress().to_string(), "->localhost recv MsgRouter_SetAddress_Ntf()");
			return;
		}
		LinkManager::onRouterAccept(session->address);
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, "exception(error_code:", e.error_code(), ", message:", e.what(), ")");
	}
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
