#include "EchoMessageHandler.h"

EchoMessageHandler::EchoMessageHandler()
{
}

EchoMessageHandler::~EchoMessageHandler()
{
}

void EchoMessageHandler::Recv_CliSvr_Req(const std::shared_ptr<EchoSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_EchoMessage_Req req;
	MsgSvrCli_EchoMessage_Ans ans;
	MsgSvrSvr_EchoMessage_Ntf ntf;
	ans.Error = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_EchoMessage_Req(session_key:", session->session_key, ")");
		ans.Message = req.Message;
		ntf.Message = req.Message;
		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::MULTI_CAST, "Echo", 0);
		Gamnet::Network::Router::SendMsg(dest, ntf);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_EchoMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ans.Error, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	EchoSession,
	MsgCliSvr_EchoMessage_Req,
	EchoMessageHandler, Recv_CliSvr_Req,
	HandlerStatic
);

void EchoMessageHandler::Recv_SvrSvr_Ntf(const Gamnet::Network::Router::Address& addr, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrSvr_EchoMessage_Ntf ntfFromSvr;

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfFromSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
			return;
		}

		LOG(DEV, "MsgSvrSvr_EchoMessage_Ntf(message:", ntfFromSvr.Message, ")");
		
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_EchoMessage_Ntf,
	EchoMessageHandler, Recv_SvrSvr_Ntf,
	HandlerStatic
);

void Test_EchoMessage_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_EchoMessage_Req req;
	req.Message = "Hello World";
	Gamnet::Test::SendMsg(session, req);
}

void Test_EchoMessage_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_EchoMessage_Ans ans;
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
	TestSession, "Test_EchoMessage",
	MsgCliSvr_EchoMessage_Req, MsgSvrCli_EchoMessage_Ans,
	Test_EchoMessage_Req, Test_EchoMessage_Ans
);

void Test_EchoMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_EchoMessage_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_EchoMessage_Ntf, Test_EchoMessage_Ntf
);

