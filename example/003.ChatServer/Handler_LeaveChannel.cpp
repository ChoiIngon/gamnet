#include "Handler_LeaveChannel.h"
#include "Manager_CastGroup.h"

Handler_LeaveChannel::Handler_LeaveChannel() {
}

Handler_LeaveChannel::~Handler_LeaveChannel() {
}

void Handler_LeaveChannel::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_LeaveChannel_Req req;
	MsgSvrCli_LeaveChannel_Ans ans;
	MsgSvrCli_LeaveChannel_Ntf ntf;

	ans.ErrorCode = GErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}

		std::shared_ptr<GUserData> userData = session->component->GetComponent<GUserData>();
		if (nullptr == userData)
		{
			throw GAMNET_EXCEPTION(GErrorCode::InvalidUserError);
		}

		if (nullptr == session->chat_channel)
		{
			throw GAMNET_EXCEPTION(GErrorCode::CanNotCreateCastGroup);
		}

		ntf.LeaveUserData = *userData;

		Gamnet::Network::Tcp::CastGroup::LockGuard lockedPtr(session->chat_channel);
		lockedPtr->Remove(session);
		Gamnet::Network::Tcp::SendMsg(session, ans);
		lockedPtr->SendMsg(ntf);
		session->chat_channel = nullptr;
		return;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.ErrorCode = (GErrorCode)e.error_code();
	}
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_LeaveChannel_Req,
	Handler_LeaveChannel, Recv_Req,
	HandlerStatic
);

void Test_LeaveChannel_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_LeaveChannel_Req req;
	LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_LeaveChannel_Req()");
	Gamnet::Test::SendMsg(session, req);
}

void Test_LeaveChannel_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_LeaveChannel_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_LeaveChannel_Ans(error_code:", ToString<GErrorCode>(ans.ErrorCode), ", channel_seq:", session->channel_seq, ")");
		session->Next();
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_LeaveChannel",
	MsgCliSvr_LeaveChannel_Req, Test_LeaveChannel_Req, 
	MsgSvrCli_LeaveChannel_Ans, Test_LeaveChannel_Ans
);

GAMNET_BIND_TEST_RECV_HANDLER(TestSession, "", MsgSvrCli_LeaveChannel_Ans, Test_LeaveChannel_Ans);

void Test_LeaveChannel_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_LeaveChannel_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(GErrorCode::MessageFormatError, "message load fail");
		}
		session->user_ids.erase(ntf.LeaveUserData.UserID);
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_LeaveChannel_Ntf(user_id:", ntf.LeaveUserData.UserID, ", channel_seq:", session->channel_seq, ", user_count:", session->user_ids.size(), ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "", MsgSvrCli_LeaveChannel_Ntf, Test_LeaveChannel_Ntf
);
