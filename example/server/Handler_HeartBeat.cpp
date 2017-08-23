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
		/*
		if(ntf.msg_seq > session->user_data->msg_seq && 1 < ntf.msg_seq - session->user_data->msg_seq)
		{
			throw Gamnet::Exception(GAMNET_ERRNO(MessageSeqOmmitError), "lost message seq:", session->user_data->msg_seq);
		}
		session->user_data->msg_seq = max(session->user_data->msg_seq, ntf.msg_seq);
		session->user_data->kickout_time = time(NULL) + 300;

		if(10 < session->user_data->msg_seq - session->ack_seq )
		{
			Gamnet::Network::Tcp::ServerState<Session>("server");
			session->ack_seq = max(session->ack_seq, session->user_data->msg_seq);
			MsgSvrCli_HeartBeat_Ntf ans;
			ans.msg_seq = session->user_data->msg_seq;
			LOG(DEV, "MsgSvrCli_HeartBeat_Ntf(msg_seq:", ans.msg_seq, ")");
			Gamnet::Network::Tcp::SendMsg(session, ans);
		}
		*/
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
