#include "RouterHandler.h"
#include "../../Log/Log.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "LinkManager.h"
namespace Gamnet { namespace Network { namespace Router {

void RouterHandler::Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SendMsg_Ntf ntf;
	try {
		if(false == Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
		}

		/*
		if(nullptr == Singleton<RouterCaster>::GetInstance().FindSession(session->address))
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, " receive a message from unregistered address(ip:", session->remote_address->to_string(), ", service_name:", session->address.service_name, ")");
		}
		*/

		Address addr = session->address;
		addr.msg_seq = ntf.msg_seq;
		std::shared_ptr<Network::Tcp::Packet> session_packet = Network::Tcp::Packet::Create();
		if (nullptr == session_packet)
		{
			LOG(GAMNET_ERR, "can not create packet");
			return;
		}
		
		session_packet->Append(ntf.buffer.c_str(), ntf.buffer.length());
		session_packet->ReadHeader();
		Singleton<Dispatcher>::GetInstance().OnRecvMsg(addr, session_packet);
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(", e.error_code(), ")");
	}
}
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
		LOG(INF, "[Router] recv SetAddress_Req (", session->remote_address->to_string(), "->localhost, service_name:", req.local_address.service_name.c_str(), ")");

		if (Singleton<LinkManager>::GetInstance().local_address == req.local_address)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "same router address(", req.local_address.service_name, ":", (int)req.local_address.cast_type, ":", req.local_address.id, ")");
		}
		session->address = req.local_address;
		ans.remote_address = Singleton<LinkManager>::GetInstance().local_address;
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Router] send SetAddress_Ans (localhost->", session->remote_address->to_string(), ", service_name:", req.local_address.service_name.c_str(), ")");
	SendMsg(session, ans);
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
			std::shared_ptr<Link> _link = session->link;
			_link->strand.wrap(std::bind(&Link::Close, _link, ans.error_code))();
			throw Exception(ans.error_code, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] Recv_SetAddress_Ans fail");
		}
		LOG(GAMNET_INF, "[Router] recv SetAddress_Ans(", session->remote_address->to_string(), "->localhost, service_name:", ans.remote_address.service_name, ")");
		Singleton<RouterCaster>::GetInstance().RegisterAddress(ans.remote_address, session);
		if (Singleton<LinkManager>::GetInstance().local_address != ans.remote_address)
		{
			Log::Write(GAMNET_INF, "[Router] send SetAddress_Ntf (localhost->", session->remote_address->to_string(), ")");
			MsgRouter_SetAddress_Ntf ntf;
			SendMsg(session, ntf);
			std::lock_guard<std::mutex> lo(LinkManager::lock);
			session->onRouterConnect(session->address);
		}
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
		LOG(GAMNET_INF, "[Router] recv SetAddress_Ntf (", session->remote_address->to_string(), "->localhost)");
		Singleton<RouterCaster>::GetInstance().RegisterAddress(session->address, session);
		std::lock_guard<std::mutex> lo(LinkManager::lock);
		LinkManager::onRouterAccept(session->address);
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, "exception(error_code:", e.error_code(), ", message:", e.what(), ")");
	}
}

void RouterHandler::Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	LOG(DEV, "[Router] recv heartbeat message(address:", session->address.service_name, ":", (int)session->address.cast_type, ":", session->address.id,")");
}

}}}
