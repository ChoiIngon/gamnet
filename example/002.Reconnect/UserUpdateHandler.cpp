#include "UserUpdateHandler.h"

UserUpdateHandler::UserUpdateHandler()
{
}

UserUpdateHandler::~UserUpdateHandler()
{
}

void UserUpdateHandler::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_UserUpdate_Req req;
	MsgSvrCli_UserUpdate_Ans ans;
	ans.Error = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_UserUpdate_Req(session_key:", session->session_key, ")");
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_UserUpdate_Ans(session_key:", session->session_key, ", error_code:", (int)ans.Error, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_UserUpdate_Req,
	UserUpdateHandler, Recv_Req,
	HandlerStatic
);

void Test_UserUpdate_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_UserUpdate_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_UserUpdate_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_UserUpdate_Ans ans;
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
	TestSession, "Test_UserUpdate",
	MsgCliSvr_UserUpdate_Req, MsgSvrCli_UserUpdate_Ans,
	Test_UserUpdate_Req, Test_UserUpdate_Ans
);

