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
			throw Gamnet::Exception(GAMNET_ERRNO(MessageFormatError), "message load fail");
		}

		LOG(DEV, "MsgCliSvr_HeartBeat_Ntf(session_key:", session->session_key, ", msg_seq:", ntf.msg_seq, ")");
		if(0 == session->user_data.user_seq)
		{
			throw Gamnet::Exception(GAMNET_ERRNO(InvalidUserError), "invalid user(session_key:", session->session_key, ")");
		}
		
		if(1 != ntf.msg_seq - session->ack_seq)
		{
			throw Gamnet::Exception(GAMNET_ERRNO(MessageSeqOmmitError), "(recv:", ntf.msg_seq, ", expect:", session->ack_seq + 1);
		}
		session->ack_seq = max(session->ack_seq, ntf.msg_seq);
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		MsgSvrCli_Kickout_Ntf ntf;
		ntf.error_code = (ErrorCode)e.error_code();
		Gamnet::Network::Tcp::SendMsg(session, ntf);
	}
}

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_HeartBeat_Ntf,
	Handler_HeartBeat, Recv_Ntf,
	HandlerStatic
);
