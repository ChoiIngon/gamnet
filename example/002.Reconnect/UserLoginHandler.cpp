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

		LOG(DEV, "MsgCliSvr_UserLogin_Req(session_key:", session->session_key, ")");
		/*
		if (0 != session->user_data.UserSEQ)
		{
			throw GAMNET_EXCEPTION(ErrorCode::AlreadyLoginSessionError, "session_key:", session->session_key);
		}

		const std::shared_ptr<Session> otherSession = Gamnet::Singleton<Manager_Session>::GetInstance().Add(req.user_id, session);
		if (nullptr != otherSession)
		{
			LOG(WRN, "duplicated session(user_id:", req.user_id, ")");
			MsgSvrCli_Kickout_Ntf ntf;
			ntf.error_code = ErrorCode::DuplicateConnectionError;
			LOG(DEV, "MsgSvrCli_Kickout_Ntf(session_key:", otherSession->session_key, ")");
			Gamnet::Network::Tcp::SendMsg(otherSession, ntf, true);
			otherSession->user_data.user_seq = 0;
		}

		*/
		UserData& userData = session->user_data;
		userData.UserID = req.UserID;
		ans.User = session->user_data;
		session->handover_safe = true;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_UserLogin_Ans(session_key:", session->session_key, ", error_code:", (int)ans.Error, ")");
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
	Gamnet::Test::SendMsg(session, req);
	session->Reconnect();
}

void Test_UserLogin_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_UserLogin_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_UserLogin",
	MsgCliSvr_UserLogin_Req, MsgSvrCli_UserLogin_Ans,
	Test_UserLogin_Req, Test_UserLogin_Ans
);

