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
		if(false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "router message format error");
		}

		if(NULL == Singleton<RouterCaster>::GetInstance().FindSession(session->addr))
		{
			throw Gamnet::Exception(GAMNET_ERRNO(ErrorCode::InvalidAddressError), " receive a message from unregistered address(ip:", session->remote_address->to_string(), ", service_name:", session->addr.service_name, ")");
		}
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(", e.error_code(), ")");
		return;
	}

	Address addr = session->addr;
	addr.msg_seq = ntf.nKey;
	std::shared_ptr<Network::Tcp::Packet> session_packet = Network::Tcp::Packet::Create();
	session_packet->Clear();
	session_packet->Append(ntf.sBuf.c_str(), ntf.sBuf.length());
	Singleton<Dispatcher>::GetInstance().OnRecvMsg(addr, session_packet);
}
void RouterHandler::Recv_SetAddress_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Req req;
	MsgRouter_SetAddress_Ans ans;
	ans.nErrorCode = ErrorCode::Success;

	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "router message format error");
		}
		LOG(INF, "[Router] recv SetAddress_Req (", session->remote_address->to_string(), "->localhost, service_name:", req.tLocalAddr.service_name.c_str(), ")");

		if (Singleton<LinkManager>::GetInstance().localAddr_ == req.tLocalAddr)
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::InvalidAddressError), "same router address(", req.tLocalAddr.service_name, ":", req.tLocalAddr.cast_type, ":", req.tLocalAddr.id, ")");
		}
		session->addr = req.tLocalAddr;
		ans.tRemoteAddr = Singleton<LinkManager>::GetInstance().localAddr_;
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(", e.error_code(), ")");
		ans.nErrorCode = e.error_code();
	}

	LOG(INF, "[Router] send SetAddress_Ans (localhost->", session->remote_address->to_string(), ", service_name:", req.tLocalAddr.service_name.c_str(), ")");
	SendMsg(session, ans);
}
void RouterHandler::Recv_SetAddress_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Ans ans;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "router message format error");
		}

		if(ErrorCode::Success != ans.nErrorCode)
		{
			std::shared_ptr<Link> _link = session->link;
			_link->OnError(ans.nErrorCode);
#ifdef _WIN32
			throw Gamnet::Exception(ans.nErrorCode, "ERR [", __FILE__, ":", __FUNCTION__, "@", __LINE__, "] Recv_SetAddress_Ans fail");
#else
			throw Gamnet::Exception(ans.nErrorCode, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] Recv_SetAddress_Ans fail");
#endif
		}
		LOG(GAMNET_INF, "[Router] recv SetAddress_Ans(", session->remote_address->to_string(), "->localhost, service_name:", ans.tRemoteAddr.service_name, ")");
		Singleton<RouterCaster>::GetInstance().RegisterAddress(ans.tRemoteAddr, session);
		if (Singleton<LinkManager>::GetInstance().localAddr_ != ans.tRemoteAddr)
		{
			Log::Write(GAMNET_INF, "[Router] send SetAddress_Ntf (localhost->", session->remote_address->to_string(), ")");
			MsgRouter_SetAddress_Ntf ntf;
			SendMsg(session, ntf);
			std::lock_guard<std::mutex> lo(LinkManager::lock);
			session->onRouterConnect(session->addr);
		}
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(", e.error_code() ,")");
	}
}
void RouterHandler::Recv_SetAddress_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	MsgRouter_SetAddress_Ntf ntf;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "router message format error");
		}
		LOG(GAMNET_INF, "[Router] recv SetAddress_Ntf (", session->remote_address->to_string(), "->localhost)");
		Singleton<RouterCaster>::GetInstance().RegisterAddress(session->addr, session);
		std::lock_guard<std::mutex> lo(LinkManager::lock);
		LinkManager::onRouterAccept(session->addr);
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, "exception(error_code:", e.error_code(), ", message:", e.what(), ")");
	}
}

}}}
