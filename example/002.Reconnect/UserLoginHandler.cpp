#include "UserLoginHandler.h"

UserLoginHandler::UserLoginHandler()
{
}

UserLoginHandler::~UserLoginHandler()
{
}

void UserLoginHandler::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_UserLogin_Req req;
	MsgSvrCli_UserLogin_Ans ans;
	ans.Error = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_UserLogin_Req");
		
		UserData& userData = session->user_data;
		userData.UserID = req.UserID;
		ans.User = session->user_data;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_UserLogin_Ans(error_code:", (int)ans.Error, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans, true);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_UserLogin_Req,
	UserLoginHandler, Recv_Req,
	HandlerStatic
);

void Test_UserLogin_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_UserLogin_Req req;
	req.UserID = "UserID";
	LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Req");
	Gamnet::Test::SendMsg(session, req);
}

void Test_UserLogin_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_UserLogin_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_UserLogin",
	MsgCliSvr_UserLogin_Req, MsgSvrCli_UserLogin_Ans,
	Test_UserLogin_Req, Test_UserLogin_Ans
);

