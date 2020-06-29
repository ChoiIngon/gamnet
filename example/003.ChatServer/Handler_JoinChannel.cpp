#include "Handler_JoinChannel.h"
#include "Manager_CastGroup.h"

Handler_JoinChannel::Handler_JoinChannel() {
}

Handler_JoinChannel::~Handler_JoinChannel() {
}

void Handler_JoinChannel::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_JoinChannel_Req req;
	MsgSvrCli_JoinChannel_Ans ans;
	MsgSvrCli_JoinChannel_Ntf ntf;

	ans.error_code = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_JoinChannel_Req()");
		std::shared_ptr<UserData> userData = session->component->GetComponent<UserData>();
		if(nullptr == userData)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
		}

		std::shared_ptr<Gamnet::Network::Tcp::CastGroup> castGroup = Gamnet::Singleton<Manager_CastGroup>::GetInstance().JoinCastGroup(session);
		if(nullptr == castGroup)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}

		Gamnet::Network::Tcp::CastGroup::LockGuard lockedPtr(castGroup);
		ans.channel_seq = lockedPtr->group_seq;
		for(auto& itr : lockedPtr->sessions)
		{
			std::shared_ptr<UserSession> existSession = std::static_pointer_cast<UserSession>(itr.second);
			std::shared_ptr<UserData> existUserData = existSession->component->GetComponent<UserData>();
			if (nullptr == existUserData)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
			}

			ans.user_datas.push_back(*existUserData);
		}		

		ntf.user_count = lockedPtr->Size();
		ntf.new_user_data = *userData;
		session->chat_channel = castGroup;
		
		Gamnet::Network::Tcp::SendMsg(session, ans);
		lockedPtr->SendMsg(ntf);

		return;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_JoinChannel_Req,
	Handler_JoinChannel, Recv_Req,
	HandlerStatic
);

void Test_JoinChannel_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_JoinChannel_Req req;
	//LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_JoinChannel_Req()");
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
		//LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_JoinChannel_Ans(error_code:", ToString<ErrorCode>(ans.ErrorCode), ", channel_seq:", ans.ChannelSEQ, ")");

		session->channel_seq = ans.channel_seq;
		for(auto& userData : ans.user_datas)
		{
			session->user_ids.insert(userData.user_id);
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_JoinChannel",
	MsgCliSvr_JoinChannel_Req, Test_JoinChannel_Req, 
	MsgSvrCli_JoinChannel_Ans, Test_JoinChannel_Ans
);

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "", 
	MsgSvrCli_JoinChannel_Ans, Test_JoinChannel_Ans
);

void Test_JoinChannel_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_JoinChannel_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		session->user_ids.insert(ntf.new_user_data.user_id);
		if(CHAT_QUORUM <= ntf.user_count)
		{
			session->Next();
		}
		//LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_JoinChannel_Ntf(channel_seq:", session->channel_seq, ", user_id:", ntf.NewUserData.UserID, ", user_count:", session->user_ids.size(), ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(TestSession, "", MsgSvrCli_JoinChannel_Ntf,	Test_JoinChannel_Ntf);
