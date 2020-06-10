#include "Handler_SendMessage.h"

Handler_SendMessage::Handler_SendMessage() {
}

Handler_SendMessage::~Handler_SendMessage() {
}

void Handler_SendMessage::Recv_CliSvr_Ntf(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_SendMessage_Ntf ntfCliSvr;
	MsgSvrSvr_SendMessage_Ntf ntfSvrSvr;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfCliSvr, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_SendMessage_Ntf(session_key:", session->session_key, ", message:", ntfCliSvr.Message, ")");
		ntfSvrSvr.Message = ntfCliSvr.Message;
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
	HandlerStatic
);

void Handler_SendMessage::Recv_SvrSvr_Ntf(const Gamnet::Network::Router::Address& address, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrSvr_SendMessage_Ntf ntfSvrSvr;
	MsgSvrCli_SendMessage_Ntf ntfSvrCli;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfSvrSvr, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}
		LOG(INF, "RECV MsgSvrSvr_SendMessage_Ntf(from:", address.service_name, ", to:", Gamnet::Network::Router::GetRouterAddress().service_name, ", message:", ntfSvrSvr.Message, ")");
		ntfSvrCli.Message = ntfSvrSvr.Message;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	LOG(INF, "SEND MsgSvrCli_SendMessage_Ntf(from:", Gamnet::Network::Router::GetRouterAddress().service_name, ", message:", ntfSvrCli.Message, ")");
	Gamnet::Singleton<Manager_Session>::GetInstance().SendMsg(ntfSvrCli);
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Ntf, 
	Handler_SendMessage, Recv_SvrSvr_Ntf, 
	HandlerStatic
);

void Test_CliSvr_SendMessage_Ntf(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Ntf ntf;
	ntf.Message = "Hello World";
	//LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_SendMessage_Ntf(message:", ntf.Message, ")");
	Gamnet::Test::SendMsg(session, ntf);
}

void Test_SvrCli_SendMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
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

/*
static std::mutex lock;
static std::set<Gamnet::Network::Router::Address> addresses;
static Gamnet::Timer timer;
void StartRouterMessageTimer()
{
	timer.AutoReset(false);
	timer.SetTimer(10000, []() {
		std::lock_guard<std::mutex> lo(lock);
		for (auto& itr : addresses)
		{
			MsgSvrSvr_SendMessage_Req req;
			req.Message = "Hello World";
			LOG(INF, "SEND MsgSvrSvr_SendMessage_Req(from:", Gamnet::Network::Router::GetRouterAddress().service_name, ", to:", itr.service_name, ", message:", req.Message, ")");
			Gamnet::Network::Router::SendMsg(itr, req);
		}
		StartRouterMessageTimer();
	});
}

*/