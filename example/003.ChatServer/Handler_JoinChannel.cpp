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
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_JoinChannel_Req");

		session->chat_channel = Gamnet::Singleton<Manager_CastGroup>::GetInstance().GetCastGroup();
		if(nullptr == session->chat_channel)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}
		 
		session->chat_channel->AddSession(session);
		/*
		if(2 > session->chat_channel->Size())
		{
			Gamnet::Singleton<Manager_CastGroup>::GetInstance().AddCastGroup(session->chat_channel);
		}
		*/

		MsgSvrCli_JoinChannel_Ntf ntf;
		ntf.NewUserData = session->user_data;
		session->chat_channel->SendMsg(ntf, true);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_JoinChannel_Ans(error_code:", ToString<ErrorCode>(ans.Error), ")");
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
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_JoinChannel",
	MsgCliSvr_JoinChannel_Req, MsgSvrCli_JoinChannel_Ans,
	Test_JoinChannel_Req, Test_JoinChannel_Ans
);

void Test_JoinChannel_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	{
		MsgSvrCli_JoinChannel_Ntf ntf;
		try {
			if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
			}
		}
		catch (const Gamnet::Exception& e) {
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_JoinChannel_Ntf,	Test_JoinChannel_Ntf
);
