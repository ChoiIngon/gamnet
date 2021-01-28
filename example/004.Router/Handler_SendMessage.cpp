#include "Handler_SendMessage.h"

static std::atomic<uint32_t> MESSAGE_SEQ;

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
		LOG(INF, "--- [RECV] MsgCliSvr_SendMessage_Req(session_key:", session->session_key, ", message:", reqCliSvr.text, ")");
		reqSvrSvr.text = reqCliSvr.text;

		std::string serviceName = "ROUTER";
		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::ANY_CAST, serviceName, 0);
		
		/*
		MsgSvrSvr_SendMessage_Ans ansSvrSvr;
		LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Req(session_key:", session->session_key, ", router_address:", dest.ToString(), ", message:", reqSvrSvr.text, ")");
		Gamnet::Network::Router::SendMsg(dest, reqSvrSvr, ansSvrSvr, 5);
		LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrSvr.error_code, ")");
		ansSvrCli.error_code = ansSvrSvr.error_code;
		*/

		LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Req(session_key:", session->session_key, ", router_address:", dest.ToString(), ", message:", reqSvrSvr.text, ")");
		Gamnet::Network::Router::SendMsg(dest, reqSvrSvr, BindResponse<MsgSvrSvr_SendMessage_Ans>(5000, &Handler_SendMessage::Recv_SvrSvr_Ans));
		return;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrCli.error_code = (ErrorCode)e.error_code();
	}
	LOG(INF, "--- [SEND] MsgSvrCli_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrCli.error_code, ")");
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
		LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Req(router_address:", address.ToString(), ", message:", reqSvrSvr.text, ")");
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrSvr.error_code = (ErrorCode)e.error_code();
	}
	LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Ans(router_address:", address.ToString(), ", error_code:", (int)ansSvrSvr.error_code, ")");
	Gamnet::Network::Router::SendMsg(address, ansSvrSvr);
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Req,
	Handler_SendMessage, Recv_SvrSvr_Req,
	HandlerStatic
);

void Handler_SendMessage::Recv_SvrSvr_Ans(const MsgSvrSvr_SendMessage_Ans& ansSvrSvr)
{
	std::shared_ptr<UserSession> session = this->session.lock();
	session->Dispatch([this, session, ansSvrSvr]() {
		MsgSvrCli_SendMessage_Ans ansSvrCli;
		ansSvrCli.error_code = ErrorCode::Success;
		try {
			ansSvrCli.error_code = ansSvrSvr.error_code;
			LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrSvr.error_code, ")");
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
		LOG(INF, "--- [SEND] MsgSvrCli_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrSvr.error_code, ")");
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
		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::ANY_CAST, "ROUTER_1", 0);
		Gamnet::Network::Router::SendMsg(dest, ntfSvrSvr);
	}
	{
		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::MULTI_CAST, "ROUTER_2", 0);
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
	//LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_SendMessage_Ntf(message:", ntf.Message, ")");
	session->pause_timer = Gamnet::Time::Timer::Create();
	session->pause_timer->SetTimer(3000, [session, req]() {
		Gamnet::Test::SendMsg(session, req);
	});
}

void Test_SvrCli_SendMessage_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
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
	TestSession, "Test_SendMessage_WithResponse",
	MsgCliSvr_SendMessage_Req, Test_CliSvr_SendMessage_Req,
	MsgSvrCli_SendMessage_Ans, Test_SvrCli_SendMessage_Ans
);

void Test_CliSvr_SendMessage_Ntf(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Ntf ntf;
	ntf.text = "Hello World";
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