#include "Handler_HeartBeat.h"
#include <algorithm>

Handler_HeartBeat::Handler_HeartBeat() {
}

Handler_HeartBeat::~Handler_HeartBeat() {
}

void Handler_HeartBeat::Recv_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_HeartBeat_Ntf ntf;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_HeartBeat_Ntf(session_key:", session->session_key, ", msg_seq:", ntf.msg_seq, ")");
		if(0 == session->user_data.user_seq)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError, "invalid user(session_key:", session->session_key, ")");
		}
	/*	
		if(1 != ntf.msg_seq - session->ack_seq)
		{
			throw Gamnet::Exception(GAMNET_ERRNO(ErrorCode::MessageSeqOmmitError), "(recv:", ntf.msg_seq, ", expect:", session->ack_seq + 1, ")");
		}
		session->ack_seq = ntf.msg_seq;
	*/
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		MsgSvrCli_Kickout_Ntf ntfToCli;
		ntfToCli.error_code = e.error_code();
		Gamnet::Network::Tcp::SendMsg(session, ntfToCli);
	}
	MsgSvrCli_HeartBeat_Ntf ntfToCli;
	LOG(DEV, "MsgSvrCli_HeartBeat_Ntf(session_key:", session->session_key, ", msg_seq:", ntf.msg_seq, ")");
	Gamnet::Network::Tcp::SendMsg(session, ntfToCli);
}

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_HeartBeat_Ntf,
	Handler_HeartBeat, Recv_Ntf,
	HandlerStatic
);

static std::atomic_uint msg_seq;
void Test_CliSvr_HeartBeat_Ntf(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_HeartBeat_Ntf ntf;
	ntf.msg_seq = ++msg_seq;
	Gamnet::Test::SendMsg(session, ntf);
}

void Test_SvrCli_HeartBeat_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_HeartBeat_Ntf ntf;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_HeartBeat",
	MsgCliSvr_HeartBeat_Ntf, MsgSvrCli_HeartBeat_Ntf,
	Test_CliSvr_HeartBeat_Ntf, Test_SvrCli_HeartBeat_Ntf
);
