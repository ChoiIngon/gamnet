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
			throw Gamnet::Exception(ERROR(ERROR_INVALID_MSG_FORMAT), "message load fail");
		}

		if(NULL == session->user_data)
		{
			throw Gamnet::Exception(ERROR(ERROR_INVALID_USER), "invalid user");
		}

		LOG(DEV, "MsgCliSvr_HeartBeat_Ntf(user_id:", session->user_data->user_id, ", msg_seq:", ntf.msg_seq, ")");
		if(1 < ntf.msg_seq - session->user_data->msg_seq)
		{
			throw Gamnet::Exception(ERROR(ERROR_INCORRECT_DATA), "lost message seq:", session->user_data->msg_seq);
		}
		session->user_data->msg_seq = std::max(session->user_data->msg_seq, ntf.msg_seq);
		session->user_data->kickout_time = time(NULL) + 300;
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		MsgSvrCli_Kickout_Ntf ntf;
		ntf.error_code = (ERROR_CODE)e.error_code();
		Gamnet::Network::SendMsg(session, ntf);
	}
}

GAMNET_BIND_NETWORK_HANDLER(
	Session,
	MsgCliSvr_HeartBeat_Ntf,
	Handler_HeartBeat, Recv_Ntf,
	HandlerStatic
);
