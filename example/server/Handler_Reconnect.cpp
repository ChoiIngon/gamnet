#include "Handler_Reconnect.h"
#include "Manager_Session.h"
Handler_Reconnect::Handler_Reconnect() {
}

Handler_Reconnect::~Handler_Reconnect() {
}

void Handler_Reconnect::Recv_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Reconnect_Req req;
	MsgSvrCli_Reconnect_Ans ans;
	ans.error_code = Success;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw Gamnet::Exception(ERROR(MessageFormatError), "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Reconnect_Req(session_key:", session->session_key, ", user_id:", req.user_id, ", access_token:", req.access_token, ")");

		if(NULL != session->user_data)
		{
			throw Gamnet::Exception(ERROR(AlreadyLoginSessionError), "session_key:", session->session_key);
		}
/*
		const std::shared_ptr<UserData> user_data = std::shared_ptr<UserData>(new UserData());
		user_data->session_key = session->sessionKey_;
		user_data->kickout_time = ::time(NULL) + 300;

		const std::shared_ptr<UserData> old_ = Gamnet::Singleton<Manager_Session>::GetInstance().Add(req.user_id, user_data);
		if(NULL == old_)
		{
			throw Gamnet::Exception(ERROR(ReconnectTimeoutError), "no cached user_data(user_id:", req.user_id, ")");
		}

		const std::shared_ptr<Session> other = Gamnet::Network::FindSession<Session>(old_->session_key);
		if(NULL != other)
		{
			MsgSvrCli_Kickout_Ntf ntf;
			ntf.error_code = DuplicateConnectionError;
			LOG(DEV, "MsgSvrCli_Kickout_Ntf(session_key:", other->sessionKey_, ")");
			Gamnet::Network::SendMsg(other, ntf);
			other->strand_.wrap(std::bind(&Session::OnAccept, other))();
		}

		if (req.access_token != old_->access_token)
		{
			throw Gamnet::Exception(ERROR(InvalidAccessTokenError), "(req:", req.access_token, ", cached:", old_->access_token, ")");
		}

		old_->session_key = user_data->session_key;
		old_->kickout_time = user_data->kickout_time;
		Gamnet::Singleton<Manager_Session>::GetInstance().Add(req.user_id, old_);

		session->user_data = old_;
		ans.msg_seq = session->user_data->msg_seq;
*/
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_Reconnect_Ans(user_id:", req.user_id, ", error_code:", ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_Reconnect_Req,
	Handler_Reconnect, Recv_Req,
	HandlerCreate
);
