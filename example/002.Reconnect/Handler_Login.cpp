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
	ans.error_code = ErrorCode::Success;
	ans.user_data.user_id = "";
	ans.user_data.user_seq = 0;

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(user_id:", req.user_id,")");
		
		std::shared_ptr<UserData> userData = session->AddComponent<UserData>();
		userData->user_id = req.user_id;
		userData->user_seq = ++USER_SEQ;
		userData->frame = 1;
		ans.user_data = *userData;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_Login_Ans(error_code:", (int)ans.error_code, ", user_seq:", ans.user_data.user_seq, ")");
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
	req.user_id = "UserID";
//	LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Req");
	session->socket = nullptr;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
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

