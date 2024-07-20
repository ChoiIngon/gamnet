#include "../../Log/Log.h"
#include "../../Library/Singleton.h"
#include "RouterHandler.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Router {

void RouterHandler::Recv_Connect_Req(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Req& req)
{
	SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);

	const Address&	localRouterAddr = sessionManager->local_address;
	const Address	remoteRouterAddr = Address(req.service_name, Address::CAST_TYPE::UNI_CAST, req.id);

	LOG(INF, "[Gamnet::Network::Router] ",
		"[", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> localhost:", session->socket->local_endpoint().port(), "] ",
		"RECV MsgRouter_Connect_Req(router_address:", remoteRouterAddr.ToString(), ")"
	);

	MsgRouter_Connect_Ans ans;
	ans.error_code = ErrorCode::Success;
	ans.service_name = localRouterAddr.service_name;
	ans.id = localRouterAddr.id;

	try {
		if (nullptr != Singleton<RouterCaster>::GetInstance().FindSession(remoteRouterAddr))
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress,
				"[Gamnet::Network::Router] already registered router address. session will be closed. remote_addres(",
				"router:", remoteRouterAddr.ToString(),
				", ip:", session->socket->remote_endpoint().address().to_v4().to_string(),
				", port:", session->socket->remote_endpoint().port(),
				")"
			);
		}
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		session->session_state = Network::Tcp::Session::State::Invalid;
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Gamnet::Network::Router] ",
		"[localhost:", session->socket->local_endpoint().port(), " -> ", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), "] ",
		"SEND MsgRouter_Connect_Ans(error_code:", ans.error_code, ", router_address:", localRouterAddr.ToString(), ")"
	);

	std::shared_ptr<Tcp::Packet> ansPacket = Tcp::Packet::Create();
	ansPacket->Write(ans);
	session->Network::Session::AsyncSend(ansPacket);

	if(ErrorCode::Success == ans.error_code && localRouterAddr < remoteRouterAddr)
	{
		LOG(INF, "[Gamnet::Network::Router] ",
			"[localhost:", session->socket->local_endpoint().port(), " -> ", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), "] ",
			"SEND MsgRouter_RegisterAddress_Req(router_address:", localRouterAddr.ToString(), ")"
		);
		
		MsgRouter_RegisterAddress_Req req;
		req.service_name = localRouterAddr.service_name;
		req.id = localRouterAddr.id;
		req.router_port = sessionManager->port;

		std::shared_ptr<Tcp::Packet> reqPacket = Tcp::Packet::Create();
		reqPacket->Write(req);
		session->Network::Session::AsyncSend(reqPacket);
	}
}

void RouterHandler::Recv_Connect_Ans(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Ans& ans)
{
	SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
	const Address& localRouterAddr = sessionManager->local_address;
	const Address remoteRouterAddr = Address(ans.service_name, Address::CAST_TYPE::UNI_CAST, ans.id);
	try {
		LOG(INF, "[Gamnet::Network::Router] ",
			"[", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> localhost:", session->socket->local_endpoint().port(), "] "
			"RECV MsgRouter_Connect_Ans(error_code:", ans.error_code, ", router_address:", remoteRouterAddr.ToString(), ")"
		);

		if(ErrorCode::Success != ans.error_code)
		{
			throw GAMNET_EXCEPTION(ans.error_code);
		}

		if (nullptr != Singleton<RouterCaster>::GetInstance().FindSession(remoteRouterAddr))
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateConnectionError,
				"[Gamnet::Network::Router] already registered router address. session will be closed. remote_addres(",
					"router:", remoteRouterAddr.ToString(),
					", ip:", session->socket->remote_endpoint().address().to_v4().to_string(),
					", port:", session->socket->remote_endpoint().port(),
				")"
			);
		}

		if(localRouterAddr < remoteRouterAddr)
		{
			LOG(INF, "[Gamnet::Network::Router] ",
				"[localhost:", session->socket->local_endpoint().port(), " -> ", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), "] ",
				"SEND MsgRouter_RegisterAddress_Req(router_address:", localRouterAddr.ToString(), ")"
			);

			MsgRouter_RegisterAddress_Req req;
			req.service_name = localRouterAddr.service_name;
			req.id = localRouterAddr.id;
			req.router_port = sessionManager->port;

			std::shared_ptr<Tcp::Packet> reqPacket = Tcp::Packet::Create();
			reqPacket->Write(req);
			session->Network::Session::AsyncSend(reqPacket);
		}
	}
	catch(const Exception& e)
	{
		GAMNET_LOG(Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		session->session_state = Network::Tcp::Session::State::Invalid;
		session->Close(e.error_code());
		if (ErrorCode::DuplicateRouterAddress == e.error_code())
		{
			throw e;
		}
	}
}

void RouterHandler::Recv_RegisterAddress_Req(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Req& req)
{
	MsgRouter_RegisterAddress_Ans ans;
	ans.error_code = ErrorCode::Success;

	SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
	const Address&	localRouterAddr = sessionManager->local_address;
	const Address	remoteRouterAddr = Address(req.service_name, Address::CAST_TYPE::UNI_CAST, req.id);
	try {
		LOG(INF, "[Gamnet::Network::Router] ",
			"[", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> localhost:", session->socket->local_endpoint().port(), "] "
			"RECV MsgRouter_RegisterAddress_Req(router_address:", remoteRouterAddr.ToString(), ")"
		);

		if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress(remoteRouterAddr, session))
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress,
				"duplicate router server(",
					"router_address:", remoteRouterAddr.ToString(),
					", ip:", session->socket->remote_endpoint().address().to_v4().to_string(), 
					", port:", session->socket->remote_endpoint().port(),
				")"
			);
		}

		session->remote_endpoint = session->socket->remote_endpoint();
		session->remote_endpoint.port(req.router_port);
				
		session->OnAccept();
		
		ans.service_name = localRouterAddr.service_name;
		ans.id = localRouterAddr.id;
		ans.router_port = sessionManager->port;
	}
	catch(const Exception& e) 
	{
		session->session_state = Network::Tcp::Session::State::Invalid;
		GAMNET_LOG(Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Gamnet::Network::Router] ",
		"[localhost:", session->socket->local_endpoint().port(), " -> ", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), "] ",
		"SEND MsgRouter_RegisterAddress_Ans(error_code:", (int)ans.error_code, ", router_address : ", ans.service_name, ")"
	);
	std::shared_ptr<Tcp::Packet> ansPacket = Tcp::Packet::Create();
	ansPacket->Write(ans);
	session->Network::Session::AsyncSend(ansPacket);
}

void RouterHandler::Recv_RegisterAddress_Ans(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ans& ans)
{
	const Address	remoteRouterAddr = Address(ans.service_name, Address::CAST_TYPE::UNI_CAST, ans.id);
	LOG(INF, "[Gamnet::Network::Router] ",
		"[", session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> localhost:", session->socket->local_endpoint().port(), "] "
		"RECV MsgRouter_RegisterAddress_Ans(error_code:", ans.error_code, ", router_address:", remoteRouterAddr.ToString(), ")"
	);

	if(ErrorCode::Success != ans.error_code)
	{
		session->session_state = Network::Tcp::Session::State::Invalid;
		throw GAMNET_EXCEPTION(ErrorCode::SetRouterAddressError, "error_code:", ans.error_code);
	}

	if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress(remoteRouterAddr, session ))
	{
		session->session_state = Network::Tcp::Session::State::Invalid;
		throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress );
	}
	
	session->remote_endpoint = session->socket->remote_endpoint();
	session->remote_endpoint.port(ans.router_port);
	session->OnAccept();
}

void RouterHandler::Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_SendMsg_Ntf& ntf)
{
	Singleton<Dispatcher>::GetInstance().OnReceive(session, ntf);
}

void RouterHandler::Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_HeartBeat_Ntf& ntf)
{
	LOG(INF, "[Gamnet::Network::Router] recv heartbeat message(address:", session->router_address.ToString(),")");
}

void RouterHandler::Recv_RegisterAddress_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ntf& ntf)
{
	LOG(INF, "[Gamnet::Network::Router] MsgRouter_RegisterAddress_Ntf(address:", ntf.service_name, ")");

	session->router_address = Address(ntf.service_name, Address::CAST_TYPE::UNI_CAST, ntf.id);
}
}}}
