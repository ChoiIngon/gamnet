#include "Handler_UpdateFrame.h"

Handler_UpdateFrame::Handler_UpdateFrame()
{
}

Handler_UpdateFrame::~Handler_UpdateFrame()
{
}

static std::atomic<int> USER_SEQ;

void Handler_UpdateFrame::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_UpdateFrame_Req req;
	MsgSvrCli_UpdateFrame_Ans ans;
	ans.ErrorCode = GErrorCode::Success;
	ans.Frame = 0;

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_UpdateFrame_Req()");

		std::shared_ptr<GUserData> userData = session->component->GetComponent<GUserData>();
		if(nullptr == userData)
		{
			throw GAMNET_EXCEPTION(GErrorCode::InvalidUserError);
		}
		userData->Frame++;
		ans.Frame = userData->Frame;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.ErrorCode = (GErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_UpdateFrame_Ans(error_code:", (int)ans.ErrorCode, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_UpdateFrame_Req,
	Handler_UpdateFrame, Recv_Req,
	HandlerStatic
);

void Test_UpdateFrame_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_UpdateFrame_Req req;
	//	LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUpdateFrame_Req");
	Gamnet::Test::SendMsg(session, req);
}

void Test_UpdateFrame_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_UpdateFrame_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUpdateFrame_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_UpdateFrame",
	MsgCliSvr_UpdateFrame_Req, Test_UpdateFrame_Req,
	MsgSvrCli_UpdateFrame_Ans, Test_UpdateFrame_Ans
);

