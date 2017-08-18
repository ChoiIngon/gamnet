#include "Handler_HeartBeat.h"

Handler_HeartBeat::Handler_HeartBeat() {
}

Handler_HeartBeat::~Handler_HeartBeat() {
}

void Handler_HeartBeat::Recv_Ntf(std::shared_ptr<Session> session, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	MsgCliSvr_HeartBeat_Ntf ntf;
	try {
		if(false == Gamnet::Network::Packet::Load(ntf, packet))
		{
			throw Gamnet::Exception(ERROR(MessageFormatError), "message load fail");
		}

		LOG(DEV, "MsgCliSvr_HeartBeat_Ntf(session_key:", session->sessionKey_, ", msg_seq:", ntf.msg_seq, ")");
		if(NULL == session->user_data)
		{
			throw Gamnet::Exception(ERROR(InvalidUserError), "invalid user(session_key:", session->sessionKey_, ")");
		}

		if(ntf.msg_seq > session->user_data->msg_seq && 1 < ntf.msg_seq - session->user_data->msg_seq)
		{
			throw Gamnet::Exception(ERROR(MessageSeqOmmitError), "lost message seq:", session->user_data->msg_seq);
		}
		session->user_data->msg_seq = std::max(session->user_data->msg_seq, ntf.msg_seq);
		session->user_data->kickout_time = time(NULL) + 300;

		if(10 < session->user_data->msg_seq - session->ack_seq )
		{
			session->ack_seq = std::max(session->ack_seq, session->user_data->msg_seq);
			MsgSvrCli_HeartBeat_Ntf ans;
			ans.msg_seq = session->user_data->msg_seq;
			LOG(DEV, "MsgSvrCli_HeartBeat_Ntf(msg_seq:", ans.msg_seq, ")");
			Gamnet::Network::SendMsg(session, ans);
		}
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		MsgSvrCli_Kickout_Ntf ntf;
		ntf.error_code = (ErrorCode)e.error_code();
		Gamnet::Network::SendMsg(session, ntf);
	}
}

GAMNET_BIND_NETWORK_HANDLER(
	Session,
	MsgCliSvr_HeartBeat_Ntf,
	Handler_HeartBeat, Recv_Ntf,
	HandlerStatic
);
