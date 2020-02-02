#include "Handler_SendMessage.h"
#include "Manager_CastGroup.h"

Handler_SendMessage::Handler_SendMessage() {
}

Handler_SendMessage::~Handler_SendMessage() {
}

void Handler_SendMessage::Recv_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_SendMessage_Ntf ntfCliSvr;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfCliSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		// LOG(DEV, "MsgCliSvr_SendMessage_Ntf(session_key:", session->session_key, ")");

		if (nullptr == session->cast_group)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}
		

		MsgSvrCli_SendMessage_Ntf ntfSvrCli;
		ntfSvrCli.msg_seq = ntfCliSvr.msg_seq;
		
		session->cast_group->SendMsg(ntfSvrCli);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_SendMessage_Ntf,
	Handler_SendMessage, Recv_Ntf,
	HandlerStatic
);

void Test_SendMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ntf ntfSvrCli;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfSvrCli, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	if(100 < ntfSvrCli.msg_seq)
	{
		session->Next();
		return;
	}

	MsgCliSvr_SendMessage_Ntf ntfCliSvr;
	ntfCliSvr.msg_seq = session->send_seq;
	Gamnet::Test::SendMsg(session, ntfCliSvr);
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_SendMessage_Ntf, Test_SendMessage_Ntf
);
