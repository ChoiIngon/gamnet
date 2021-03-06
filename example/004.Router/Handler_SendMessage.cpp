#include "Handler_SendMessage.h"

Handler_SendMessage::Handler_SendMessage() {
}

Handler_SendMessage::~Handler_SendMessage() {
}

void Handler_SendMessage::Recv_CliSvr_Req(const std::shared_ptr<UserSession>& session, const MsgCliSvr_SendMessage_Req& reqCliSvr)
{
	MsgSvrSvr_SendMessage_Req reqSvrSvr;
	MsgSvrCli_SendMessage_Ans ansSvrCli;
	try {
		this->session = session;
		LOG(DEV, "MsgCliSvr_SendMessage_Req(session_key:", session->session_key, ", message:", reqCliSvr.text, ")");
		reqSvrSvr.text = reqCliSvr.text;

		std::string serviceName = "ROUTER";
		Gamnet::Network::Router::Address dest(serviceName, Gamnet::Network::Router::Address::CAST_TYPE::ANY_CAST);
		
		/*
		MsgSvrSvr_SendMessage_Ans ansSvrSvr;
		LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Req(session_key:", session->session_key, ", router_address:", dest.ToString(), ", message:", reqSvrSvr.text, ")");
		Gamnet::Network::Router::SendMsg(dest, reqSvrSvr, ansSvrSvr, 5);
		LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrSvr.error_code, ")");
		ansSvrCli.error_code = ansSvrSvr.error_code;
		*/

		LOG(DEV, "MsgSvrSvr_SendMessage_Req(session_key:", session->session_key, ", message:", reqSvrSvr.text, ", to:", dest.ToString(),  ")");
		Gamnet::Network::Router::SendMsg(dest, reqSvrSvr, BindResponse<MsgSvrSvr_SendMessage_Ans>(7, &Handler_SendMessage::Recv_SvrSvr_Ans, &Handler_SendMessage::Timeout_SvrSvr_Ans));
		return;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrCli.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_SendMessage_Ans(session_key:", session->session_key, ", error_code:", ToString<ErrorCode>(ansSvrCli.error_code), ")");
	Gamnet::Network::Tcp::SendMsg(session, ansSvrCli);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_SendMessage_Req,
	Handler_SendMessage, Recv_CliSvr_Req,
	HandlerCreate
);

void Handler_SendMessage::Recv_SvrSvr_Req(const Gamnet::Network::Router::Address& address, const MsgSvrSvr_SendMessage_Req& reqSvrSvr)
{
	MsgSvrSvr_SendMessage_Ans ansSvrSvr;
	ansSvrSvr.error_code = ErrorCode::Success;
	try {
		LOG(DEV, "MsgSvrSvr_SendMessage_Req(from:", address.ToString(), ", message:", reqSvrSvr.text, ")");
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrSvr.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrSvr_SendMessage_Ans(to:", address.ToString(), ", error_code:", ToString<ErrorCode>(ansSvrSvr.error_code), ")");
	Gamnet::Network::Router::SendMsg(address, ansSvrSvr);
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Req,
	Handler_SendMessage, Recv_SvrSvr_Req,
	HandlerStatic
);

void Handler_SendMessage::Recv_SvrSvr_Ans(const Gamnet::Network::Router::Address& address, const MsgSvrSvr_SendMessage_Ans& ansSvrSvr)
{
	std::shared_ptr<UserSession> session = this->session.lock();
	if (nullptr == session)
	{
		LOG(ERR, "MsgSvrSvr_SendMessage_Ans(can't find user session)");
		return;
	}
	LOG(DEV, "MsgSvrSvr_SendMessage_Ans(from:", address.ToString(), ", error_code:", ToString<ErrorCode>(ansSvrSvr.error_code), ")");
	session->Dispatch([this, session, ansSvrSvr]() {
		MsgSvrCli_SendMessage_Ans ansSvrCli;
		ansSvrCli.error_code = ErrorCode::Success;
		try {
			ansSvrCli.error_code = ansSvrSvr.error_code;
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
		LOG(DEV, "MsgSvrCli_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrSvr.error_code, ")");
		Gamnet::Network::Tcp::SendMsg(session, ansSvrCli);
	});
}

void Handler_SendMessage::Timeout_SvrSvr_Ans(const Gamnet::Exception& e)
{
	std::shared_ptr<UserSession> session = this->session.lock();
	if (nullptr == session)
	{
		LOG(ERR, "MsgSvrSvr_SendMessage_Ans(can't find user session)");
		return;
	}

	LOG(DEV, "MsgSvrSvr_SendMessage_Ans(session_key:", session->session_key, ", error_code:", e.error_code(), ", reason:", e.what(), ")");
	session->Dispatch([this, session, e]() {
		MsgSvrCli_SendMessage_Ans ansSvrCli;
		ansSvrCli.error_code = (ErrorCode)e.error_code();
		try {
			
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
		LOG(DEV, "MsgSvrCli_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrCli.error_code, ")");
		Gamnet::Network::Tcp::SendMsg(session, ansSvrCli);
	});
}

void Handler_SendMessage::Recv_CliSvr_Ntf(const std::shared_ptr<UserSession>& session, const MsgCliSvr_SendMessage_Ntf& ntfCliSvr)
{
	MsgSvrSvr_SendMessage_Ntf ntfSvrSvr;
	try {
		LOG(INF, " --- MsgCliSvr_SendMessage_Ntf(session_key:", session->session_key, ", message:", ntfCliSvr.text, ")");
		ntfSvrSvr.text = ntfCliSvr.text;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	{
		Gamnet::Network::Router::Address dest("ROUTER_1", Gamnet::Network::Router::Address::CAST_TYPE::ANY_CAST, 0);
		Gamnet::Network::Router::SendMsg(dest, ntfSvrSvr);
	}
	{
		Gamnet::Network::Router::Address dest("ROUTER_2", Gamnet::Network::Router::Address::CAST_TYPE::MULTI_CAST, 0);
		Gamnet::Network::Router::SendMsg(dest, ntfSvrSvr);
	}
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_SendMessage_Ntf,
	Handler_SendMessage, Recv_CliSvr_Ntf,
	HandlerCreate
);

void Handler_SendMessage::Recv_SvrSvr_Ntf(const Gamnet::Network::Router::Address& address, const MsgSvrSvr_SendMessage_Ntf& ntfSvrSvr)
{
	MsgSvrCli_SendMessage_Ntf ntfSvrCli;
	try {
		LOG(INF, "RECV MsgSvrSvr_SendMessage_Ntf(from:", address.service_name, ", to:", Gamnet::Network::Router::GetRouterAddress().service_name, ", message:", ntfSvrSvr.text, ")");
		ntfSvrCli.text = ntfSvrSvr.text;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	LOG(INF, "SEND MsgSvrCli_SendMessage_Ntf(from:", Gamnet::Network::Router::GetRouterAddress().service_name, ", message:", ntfSvrCli.text, ")");
	Gamnet::Singleton<Manager_Session>::GetInstance().SendMsg(ntfSvrCli);
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Ntf, 
	Handler_SendMessage, Recv_SvrSvr_Ntf, 
	HandlerStatic
);

void Test_CliSvr_SendMessage_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Req req;
	req.text = "Hello World";
	Gamnet::Test::SendMsg(session, req);
}

void Test_SvrCli_SendMessage_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_SendMessage_WithResponse",
	MsgCliSvr_SendMessage_Req, Test_CliSvr_SendMessage_Req,
	MsgSvrCli_SendMessage_Ans, Test_SvrCli_SendMessage_Ans
);

void Test_CliSvr_SendMessage_Ntf(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Ntf ntf;
	ntf.text = "World Hello";
	//LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_SendMessage_Ntf(message:", ntf.Message, ")");
	Gamnet::Test::SendMsg(session, ntf);
}

void Test_SvrCli_SendMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_SendMessage_Ntf(message:", ntf.Message, ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_SendMessage",
	MsgCliSvr_SendMessage_Ntf, Test_CliSvr_SendMessage_Ntf, 
	MsgSvrCli_SendMessage_Ntf, Test_SvrCli_SendMessage_Ntf
);