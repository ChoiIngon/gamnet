#include "Handler_LeaveChannel.h"
#include "Manager_CastGroup.h"

Handler_LeaveChannel::Handler_LeaveChannel() {
}

Handler_LeaveChannel::~Handler_LeaveChannel() {
}

void Handler_LeaveChannel::Recv_Req(const std::shared_ptr<ChatSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_LeaveChannel_Req req;
	MsgSvrCli_LeaveChannel_Ans ans;
	ans.Error = ErrorCode::Success;
	MsgSvrCli_LeaveChannel_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		if (nullptr == session->chat_channel)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}

		ntf.LeaveUserData = session->user_data;

		Gamnet::AtomicPtr<Gamnet::Network::Tcp::CastGroup> lockedPtr(session->chat_channel);
		lockedPtr->DelSession(session);
		Gamnet::Network::Tcp::SendMsg(session, ans, true);
		lockedPtr->SendMsg(ntf, true);
		session->chat_channel = nullptr;
		return;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	Gamnet::Network::Tcp::SendMsg(session, ans, true);
}

GAMNET_BIND_TCP_HANDLER(
	ChatSession,
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
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_LeaveChannel_Ans(error_code:", ToString<ErrorCode>(ans.Error), ", channel_seq:", session->channel_seq, ")");
		session->Next();
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_LeaveChannel",
	MsgCliSvr_LeaveChannel_Req, MsgSvrCli_LeaveChannel_Ans,
	Test_LeaveChannel_Req, Test_LeaveChannel_Ans
);

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_LeaveChannel_Ans, Test_LeaveChannel_Ans
);

void Test_LeaveChannel_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_LeaveChannel_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		session->user_ids.erase(ntf.LeaveUserData.UserID);
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_LeaveChannel_Ntf(user_id:", ntf.LeaveUserData.UserID, ", channel_seq:", session->channel_seq, ", user_count:", session->user_ids.size(), ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_LeaveChannel_Ntf, Test_LeaveChannel_Ntf
);
