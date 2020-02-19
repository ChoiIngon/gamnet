#include "Handler_JoinChannel.h"
#include "Manager_CastGroup.h"

Handler_JoinChannel::Handler_JoinChannel() {
}

Handler_JoinChannel::~Handler_JoinChannel() {
}

void Handler_JoinChannel::Recv_Req(const std::shared_ptr<ChatSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_JoinChannel_Req req;
	MsgSvrCli_JoinChannel_Ans ans;
	ans.Error = ErrorCode::Success;
	MsgSvrCli_JoinChannel_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		session->chat_channel = Gamnet::Singleton<Manager_CastGroup>::GetInstance().GetCastGroup();
		if(nullptr == session->chat_channel)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}

		ans.ChannelSEQ = session->chat_channel->group_seq;
		ntf.NewUserData = session->user_data;

		Gamnet::AtomicPtr<Gamnet::Network::Tcp::CastGroup> lockedPtr(session->chat_channel);
		for(auto& itr : lockedPtr->sessions)
		{
			std::shared_ptr<ChatSession> chatSession = std::static_pointer_cast<ChatSession>(itr.second);
			ans.ExistingUserData.push_back(chatSession->user_data);
		}		
		lockedPtr->AddSession(session);
		if(2 > lockedPtr->Size())
		{
			Gamnet::Singleton<Manager_CastGroup>::GetInstance().AddCastGroup(session->chat_channel);
		}
		Gamnet::Network::Tcp::SendMsg(session, ans, true);
		lockedPtr->SendMsg(ntf, true);
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
	MsgCliSvr_JoinChannel_Req,
	Handler_JoinChannel, Recv_Req,
	HandlerStatic
);

void Test_JoinChannel_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_JoinChannel_Req req;
	LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_JoinChannel_Req()");
	Gamnet::Test::SendMsg(session, req);
}

void Test_JoinChannel_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_JoinChannel_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_JoinChannel_Ans(error_code:", ToString<ErrorCode>(ans.Error), ", channel_seq:", ans.ChannelSEQ, ")");
		session->channel_seq = ans.ChannelSEQ;
		for(auto& userData : ans.ExistingUserData)
		{
			session->user_ids.insert(userData.UserID);
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_JoinChannel",
	MsgCliSvr_JoinChannel_Req, MsgSvrCli_JoinChannel_Ans,
	Test_JoinChannel_Req, Test_JoinChannel_Ans
);

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_JoinChannel_Ans, Test_JoinChannel_Ans
);

void Test_JoinChannel_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_JoinChannel_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		session->user_ids.insert(ntf.NewUserData.UserID);
		if(2 <= session->user_ids.size())
		{
			session->Next();
		}
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_JoinChannel_Ntf(channel_seq:", session->channel_seq, ", user_id:", ntf.NewUserData.UserID, ", user_count:", session->user_ids.size(), ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_JoinChannel_Ntf,	Test_JoinChannel_Ntf
);
