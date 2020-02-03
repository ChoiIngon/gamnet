#include "EchoMessageHandler.h"

EchoMessageHandler::EchoMessageHandler()
{
}

EchoMessageHandler::~EchoMessageHandler()
{
}

void EchoMessageHandler::Recv_Req(const std::shared_ptr<EchoSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_EchoMessage_Req req;
	MsgSvrCli_EchoMessage_Ans ans;
	ans.Error = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_EchoMessage_Req(session_key:", session->session_key, ")");
		ans.Message = req.Message;
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
	EchoMessageHandler, Recv_Req,
	HandlerStatic
);

void Test_EchoMessage_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_EchoMessage_Req req;
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

