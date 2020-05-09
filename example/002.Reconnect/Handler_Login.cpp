#include "Handler_Login.h"

Handler_Login::Handler_Login()
{
}

Handler_Login::~Handler_Login()
{
}

static std::atomic<int> USER_SEQ;

void Handler_Login::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Login_Req req;
	MsgSvrCli_Login_Ans ans;
	ans.ErrorCode = GErrorCode::Success;
	ans.UserData.UserID = "";
	ans.UserData.UserSEQ = 0;

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(user_id:", req.UserID,")");
		
		std::shared_ptr<GUserData> userData = session->component->AddComponent<GUserData>();
		userData->UserID = req.UserID;
		userData->UserSEQ = ++USER_SEQ;
		userData->Frame = 1;
		ans.UserData = *userData;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.ErrorCode = (GErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_Login_Ans(error_code:", (int)ans.ErrorCode, ", user_seq:", ans.UserData.UserSEQ, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerStatic
);

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Login_Req req;
	req.UserID = "UserID";
//	LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Req");
	Gamnet::Test::SendMsg(session, req);
	session->Reconnect();
}

void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}
//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Login",
	MsgCliSvr_Login_Req, Test_Login_Req, 
	MsgSvrCli_Login_Ans, Test_Login_Ans
);

