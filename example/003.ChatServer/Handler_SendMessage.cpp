#include "Handler_SendMessage.h"
#include "Manager_CastGroup.h"

Handler_SendMessage::Handler_SendMessage() {
}

Handler_SendMessage::~Handler_SendMessage() {
}

void Handler_SendMessage::Recv_Ntf(const std::shared_ptr<ChatSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_SendMessage_Ntf ntfCliSvr;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfCliSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		//LOG(DEV, "MsgCliSvr_SendMessage_Ntf(session_key:", session->session_key, ", message:", ntfCliSvr.ChatMessage, ")");

		if (nullptr == session->chat_channel)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CanNotCreateCastGroup);
		}
		
		MsgSvrCli_SendMessage_Ntf ntfSvrCli;
		ntfSvrCli.ChatMessage = ntfCliSvr.ChatMessage;
		std::lock_guard<Gamnet::Network::Tcp::CastGroup> lo(*session->chat_channel);
		std::shared_ptr<Gamnet::Network::Tcp::CastGroup> chatChannel = session->chat_channel;
		chatChannel->SendMsg(ntfSvrCli);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TCP_HANDLER(
	ChatSession,
	MsgCliSvr_SendMessage_Ntf,
	Handler_SendMessage, Recv_Ntf,
	HandlerStatic
);

void Test_SendMessage_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Ntf ntf;
	ntf.ChatMessage = session->user_data.UserID;
	Gamnet::Test::SendMsg(session, ntf);
}

void Test_SendMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		assert(session->user_ids.end() != session->user_ids.find(ntf.ChatMessage));
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	if(100 == session->send_seq)
	{
		session->Next();
		return;
	}

	MsgCliSvr_SendMessage_Ntf ntfCliSvr;
	ntfCliSvr.ChatMessage = session->user_data.UserID;
	Gamnet::Test::SendMsg(session, ntfCliSvr);
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_SendMessage",
	MsgCliSvr_SendMessage_Ntf, MsgSvrCli_SendMessage_Ntf,
	Test_SendMessage_Req, Test_SendMessage_Ntf
);

void Test_SendMessage_Ignore(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
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
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, MsgSvrCli_SendMessage_Ntf, Test_SendMessage_Ignore
);
