#include "Handler_JoinChannel.h"
#include "Manager_CastGroup.h"

Handler_JoinChannel::Handler_JoinChannel() {
}

Handler_JoinChannel::~Handler_JoinChannel() {
}

void Handler_JoinChannel::Recv_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_JoinChannel_Req req;
	MsgSvrCli_JoinChannel_Ans ans;
	ans.error_code = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_JoinChannel_Req(session_key:", session->session_key, ")");

		std::shared_ptr<Gamnet::Network::Tcp::CastGroup> castGroup = Gamnet::Singleton<Manager_CastGroup>::GetInstance().GetCastGroup();
		if(nullptr == castGroup)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}
		session->cast_group = castGroup;
		castGroup->AddSession(session);
		if(4 > castGroup->Size())
		{
			Gamnet::Singleton<Manager_CastGroup>::GetInstance().AddCastGroup(castGroup);
		}

		MsgSvrCli_JoinChannel_Ntf ntf;
		ntf.session_key = session->session_key;
		ntf.session_count = castGroup->Size();
		castGroup->SendMsg(ntf, true);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_JoinChannel_Ans(session_key:", session->session_key, ", error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans, true);
}

GAMNET_BIND_TCP_HANDLER(
	Session,
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
	int memberCount = 0;
	{
		MsgSvrCli_JoinChannel_Ntf ntf;
		try {
			if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
			}
			memberCount = ntf.session_count;
		}
		catch (const Gamnet::Exception& e) {
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
	}
	if(4 <= memberCount)
	{
		MsgCliSvr_SendMessage_Ntf ntf;
		ntf.msg_seq = session->send_seq;
		Gamnet::Test::SendMsg(session, ntf);
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_JoinChannel_Ntf,	Test_JoinChannel_Ntf
);
