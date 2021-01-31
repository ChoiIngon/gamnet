#include "../../Log/Log.h"
#include "../../Library/Singleton.h"
#include "RouterHandler.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Router {

void RouterHandler::Recv_Connect_Req(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Req& req)
{
	Address remote = Address(req.router_address.cast_type, req.router_address.service_name, req.router_address.id);
	MsgRouter_Connect_Ans ans;
	ans.error_code = 0;

	try {
		SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
		ans.router_address = sessionManager->local_address;

		LOG(INF, "[Gamnet::Router] ",
			session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
			"localhost:", session->socket->local_endpoint().port(), " "
			"RECV MsgRouter_Connect_Req(router_address:", remote.ToString(), ")"
		);
	}
	catch (const Exception& e) 
	{
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Gamnet::Router] ",
		"localhost:", session->socket->local_endpoint().port(), " -> ",
		session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " ",
		"SEND MsgRouter_Connect_Ans(router_address:", ans.router_address.ToString(), ")"
	);

	std::shared_ptr<Tcp::Packet> ansPacket = Tcp::Packet::Create();
	ansPacket->Write(ans);
	session->Network::Session::AsyncSend(ansPacket);

	if(ErrorCode::Success == ans.error_code && ans.router_address < remote)
	{
		if (nullptr != Singleton<RouterCaster>::GetInstance().FindSession(remote))
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress,
				"duplicate router server(",
				"router_address:", remote.ToString(),
				", ip:", session->socket->remote_endpoint().address().to_v4().to_string(), ", port:", session->socket->remote_endpoint().port(),
				")"
			);
		}

		LOG(INF, "[Gamnet::Router] ",
			"localhost:", session->socket->local_endpoint().port(), " -> ",
			session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " ",
			"SEND MsgRouter_RegisterAddress_Req(router_address:", ans.router_address.ToString(), ")"
		);
		MsgRouter_RegisterAddress_Req req;
		req.router_address = ans.router_address;

		std::shared_ptr<Tcp::Packet> reqPacket = Tcp::Packet::Create();
		reqPacket->Write(req);
		session->Network::Session::AsyncSend(reqPacket);
	}
}

void RouterHandler::Recv_Connect_Ans(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Ans& ans)
{
	const Address remote = Address(ans.router_address.cast_type, ans.router_address.service_name, ans.router_address.id);
	try {
		SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
		
		LOG(INF, "[Gamnet::Router] ",
			session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
			"localhost:", session->socket->local_endpoint().port(), " "
			"RECV MsgRouter_Connect_Ans(router_address:", remote.ToString(), ", error_code:", ans.error_code, ")"
		);

		if(ErrorCode::Success != ans.error_code)
		{
			throw GAMNET_EXCEPTION(ans.error_code);
		}

		if(sessionManager->local_address < remote)
		{
			if(nullptr != Singleton<RouterCaster>::GetInstance().FindSession(remote))
			{
				throw GAMNET_EXCEPTION(ErrorCode::DuplicateConnectionError, 
					"duplicate router server(",
					"router_address:", remote.ToString(),
					", ip:", session->socket->remote_endpoint().address().to_v4().to_string(), ", port:", session->socket->remote_endpoint().port(),
					")"
				);
			}

			LOG(INF, "[Gamnet::Router] ",
				"localhost:", session->socket->local_endpoint().port(), " -> ",
				session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " ",
				"SEND MsgRouter_RegisterAddress_Req(router_address:", remote.ToString(), ")"
			);
			MsgRouter_RegisterAddress_Req req;
			req.router_address = sessionManager->local_address;
			req.router_port = sessionManager->port;
			std::shared_ptr<Tcp::Packet> reqPacket = Tcp::Packet::Create();
			reqPacket->Write(req);
			session->Network::Session::AsyncSend(reqPacket);
		}
	}
	catch(const Exception& e)
	{
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
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

	try {
		LOG(INF, "[Gamnet::Router] ",
			session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
			"localhost:", session->socket->local_endpoint().port(), " "
			"RECV MsgRouter_RegisterAddress_Req(router_address:", ans.router_address.ToString(), ")"
		);
		/*
		if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress(remote, session ))
		{
			throw GAMNET_EXCEPTION( ErrorCode::DuplicateRouterAddress );
		}
		*/

		session->router_address = Address(req.router_address.cast_type, req.router_address.service_name, req.router_address.id);
		session->remote_endpoint = session->socket->remote_endpoint();
		session->remote_endpoint.port(req.router_port);

		session->OnAccept();
		SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
		ans.router_address = sessionManager->local_address;
		ans.router_port = sessionManager->port;
	}
	catch(const Exception& e) {
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.error_code = e.error_code();
	}

	LOG(INF, "[Gamnet::Router] ",
		session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
		"localhost:", session->socket->local_endpoint().port(), " ",
		"SEND MsgRouter_RegisterAddress_Ans(error_code:", (int)ans.error_code, ", router_address : ", ans.router_address.ToString(), ")"
	);
	std::shared_ptr<Tcp::Packet> ansPacket = Tcp::Packet::Create();
	ansPacket->Write(ans);
	session->Network::Session::AsyncSend(ansPacket);
}

void RouterHandler::Recv_RegisterAddress_Ans(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ans& ans)
{
	LOG(INF, "[Gamnet::Router] ",
		session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
		"localhost:", session->socket->local_endpoint().port(), " "
		"RECV MsgRouter_RegisterAddress_Ans(error_code:", ans.error_code, ")"
	);
	if(ErrorCode::Success != ans.error_code)
	{
		throw GAMNET_EXCEPTION(ErrorCode::SetRouterAddressError, "error_code:", ans.error_code);
	}

	/*
	if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress( remote, session ))
	{
		throw GAMNET_EXCEPTION( ErrorCode::DuplicateRouterAddress );
	}
	*/
	session->router_address = Address(ans.router_address.cast_type, ans.router_address.service_name, ans.router_address.id);
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
	LOG(INF, "[Gamnet::Router] recv heartbeat message(address:", session->router_address.ToString(),")");
}

void RouterHandler::Recv_RegisterAddress_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ntf& ntf)
{
	LOG(INF, "[Gamnet::Router] MsgRouter_RegisterAddress_Ntf(address:", ntf.router_address.ToString(), ")");
	session->router_address = ntf.router_address;
}
}}}
