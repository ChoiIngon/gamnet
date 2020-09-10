#include "../../Log/Log.h"
#include "../../Library/Singleton.h"
#include "RouterHandler.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Router {

void RouterHandler::Recv_Connect_Req(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Req& req)
{
	MsgRouter_Connect_Ans ans;
	ans.error_code = 0;

	try {
		SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
		ans.router_address = sessionManager->local_address;

		LOG(INF, "[Gamnet::Router] ",
			session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
			"localhost:", session->socket->local_endpoint().port(), " "
			"RECV MsgRouter_Connect_Req(router_address:", req.router_address.ToString(), ")"
		);

		if(sessionManager->local_address < req.router_address)
		{
			if (nullptr != Singleton<RouterCaster>::GetInstance().FindSession(req.router_address))
			{
				throw GAMNET_EXCEPTION(ErrorCode::DuplicateConnectionError, "duplicate router address");
			}
		}
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

	if(ErrorCode::Success == ans.error_code && ans.router_address < req.router_address)
	{
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
	try {
		SessionManager* sessionManager = static_cast<SessionManager*>(session->session_manager);
		
		LOG(INF, "[Gamnet::Router] ",
			session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " -> ",
			"localhost:", session->socket->local_endpoint().port(), " "
			"RECV MsgRouter_Connect_Ans(router_address:", ans.router_address.ToString(), ", error_code:", ans.error_code, ")"
		);

		if(ErrorCode::Success != ans.error_code)
		{
			throw GAMNET_EXCEPTION(ans.error_code);
		}

		if(sessionManager->local_address < ans.router_address)
		{
			if(nullptr != Singleton<RouterCaster>::GetInstance().FindSession(ans.router_address))
			{
				throw GAMNET_EXCEPTION(ErrorCode::DuplicateConnectionError, "duplicate router address");
			}

			LOG(INF, "[Gamnet::Router] ",
				"localhost:", session->socket->local_endpoint().port(), " -> ",
				session->socket->remote_endpoint().address().to_v4().to_string(), ":", session->socket->remote_endpoint().port(), " ",
				"SEND MsgRouter_RegisterAddress_Req(router_address:", ans.router_address.ToString(), ")"
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
			"RECV MsgRouter_RegisterAddress_Req(router_address:", req.router_address.ToString(), ")"
		);
		if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress( req.router_address, session ))
		{
			throw GAMNET_EXCEPTION( ErrorCode::DuplicateRouterAddress );
		}
		session->remote_endpoint = session->socket->remote_endpoint();
		session->remote_endpoint.port(req.router_port);
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

	if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress( ans.router_address, session ))
	{
		throw GAMNET_EXCEPTION( ErrorCode::DuplicateRouterAddress );
	}
	session->remote_endpoint = session->socket->remote_endpoint();
	session->remote_endpoint.port(ans.router_port);
}

void RouterHandler::Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_SendMsg_Ntf& ntf)
{
	std::shared_ptr<Network::Tcp::Packet> buffer = Network::Tcp::Packet::Create();
	if (nullptr == buffer)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
	}

	buffer->Append(ntf.buffer.data(), ntf.buffer.size());
	buffer->ReadHeader();
	buffer->msg_seq = ntf.msg_seq;
	Singleton<Dispatcher>::GetInstance().OnReceive(session, buffer);
}

void RouterHandler::Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_HeartBeat_Ntf& ntf)
{
	LOG(DEV, "[Router] recv heartbeat message(address:", session->router_address.ToString(),")");
}

void RouterHandler::Recv_RegisterAddress_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ntf& ntf)
{
	LOG(DEV, "[Gamnet::Router] MsgRouter_RegisterAddress_Ntf(address:", ntf.router_address.ToString(), ")");
	session->router_address = ntf.router_address;
	session->type = Session::TYPE::RECV;
}
}}}
