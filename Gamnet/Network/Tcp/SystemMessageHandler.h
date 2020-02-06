#ifndef _GAMNET_NETWORK_TCP_SYSTEM_MESSAGE_HANDLER_H_
#define _GAMNET_NETWORK_TCP_SYSTEM_MESSAGE_HANDLER_H_

#include "../Handler.h"

namespace Gamnet {	namespace Network {		namespace Tcp {
	enum MSG_ID {
		MsgID_CliSvr_Connect_Req = 1,
		MsgID_SvrCli_Connect_Ans = 1,
		MsgID_CliSvr_Reconnect_Req = 2,
		MsgID_SvrCli_Reconnect_Ans = 2,
		MsgID_CliSvr_HeartBeat_Req = 3,
		MsgID_SvrCli_HeartBeat_Ans = 3,
		MsgID_CliSvr_ReliableAck_Ntf = 4,
		MsgID_SvrCli_ReliableAck_Ntf = 4,
		MsgID_SvrCli_Kickout_Ntf = 5,
		MsgID_CliSvr_Close_Req = 6,
		MsgID_SvrCli_Close_Ans = 6,
		MsgID_Max = 7
	};

template <class SESSION_T>
class SystemMessageHandler : public IHandler
{
public:
	void Recv_Connect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::shared_ptr<Link> link = std::static_pointer_cast<Link>(session->link);
		assert(nullptr != link);

		LOG(DEV, "[link_key:", link->link_key, "] session_key:", session->session_key, ", session_token:", session->session_token);	

		Json::Value ans;
		ans["error_code"] = 0;
		ans["session_key"] = 0;
		ans["session_token"] = "";

		try
		{
			Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Add(session->session_key, session);

			session->OnCreate();
			session->OnAccept();
			session->handover_safe = true;

			ans["session_key"] = session->session_key;
			ans["session_token"] = session->session_token;
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

		std::shared_ptr<Packet> sendPacket = Packet::Create();
		if (nullptr == sendPacket)
		{
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPacketError))();
			return;
		}

		Json::FastWriter writer;
		std::string str = writer.write(ans);

		sendPacket->Write(MSG_ID::MsgID_SvrCli_Connect_Ans, str.c_str(), str.length());
		link->AsyncSend(sendPacket);
	}

	void Recv_Reconnect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::shared_ptr<Link> link = std::static_pointer_cast<Link>(session->link);
		assert(nullptr != link);

		Json::Value req;
		Json::Value ans;
		ans["error_code"] = 0;
		try
		{
			std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
			Json::Reader reader;

			if (false == reader.parse(json, req))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[link_key:", link->link_key, "] message format error(data:", json, ")");
			}

			uint32_t session_key = req["session_key"].asUInt();
			const std::string session_token = req["session_token"].asString();

			LOG(DEV, "[link_key:", link->link_key, "] session_key:", session_key, ", session_token:", session_token);
			const std::shared_ptr<SESSION_T> prevSession = Singleton<LinkManager<SESSION_T>>::GetInstance().FindSession(session_key);
			if (nullptr == prevSession)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionKeyError, "[link_key:", link->link_key, " ] can not find session data for reconnect(session_key:", session_key, ")");
			}

			if (session_token != prevSession->session_token)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionTokenError, "[link_key:", link->link_key, "] invalid session token(expect:", prevSession->session_token, ", receive:", session_token, ")");
			}

			std::shared_ptr<Link> invalidLink = std::static_pointer_cast<Link>(prevSession->link);
			if (nullptr != invalidLink)
			{
				invalidLink->strand.wrap([invalidLink]() {
					invalidLink->session = nullptr;
					invalidLink->Close(ErrorCode::DuplicateConnectionError);
				})();
			}

			link->session = prevSession;
			for (const std::shared_ptr<Packet>& sendPacket : prevSession->send_packets)
			{
				link->AsyncSend(sendPacket);
			}

			prevSession->AttachLink(link);
			prevSession->OnAccept();
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

		//LOG(DEV, "[link_key:", link->link_key, "] error_code:", ans["error_code"].asUInt());
		Json::FastWriter writer;
		std::string str = writer.write(ans);

		std::shared_ptr<Packet> sendPacket = Packet::Create();
		if (nullptr == sendPacket)
		{
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPacketError))();
			return;
		}

		sendPacket->Write(MSG_ID::MsgID_SvrCli_Reconnect_Ans, str.c_str(), str.length());
		link->AsyncSend(sendPacket);
	}

	void Recv_HeartBeat_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::shared_ptr<Link> link = std::static_pointer_cast<Link>(session->link);
		assert(nullptr != link);

		std::shared_ptr<Packet> sendPacket = Packet::Create();
		if (nullptr == sendPacket)
		{
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPacketError))();
			return;
		}

		Json::Value ans;
		ans["error_code"] = 0;
		ans["msg_seq"] = session->recv_seq;

		Json::FastWriter writer;
		std::string str = writer.write(ans);

		packet->Write(MSG_ID::MsgID_SvrCli_HeartBeat_Ans, str.c_str(), str.length());
		link->AsyncSend(sendPacket);
	}

	void Recv_ReliableAck_Ntf(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::shared_ptr<Link> link = std::static_pointer_cast<Link>(session->link);
		assert(nullptr != link);

		std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
		Json::Value ntf;
		Json::Reader reader;
		if (false == reader.parse(json, ntf))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[link_key:", link->link_key, "] message format error(data:", json, ")");
		}

		uint32_t ackSEQ = ntf["ack_seq"].asUInt();

		while (0 < session->send_packets.size())
		{
			const std::shared_ptr<Packet>& sendPacket = session->send_packets.front();
			if (ackSEQ < sendPacket->msg_seq)
			{
				break;
			}
			session->send_packets.pop_front();
		}
	}

	void Recv_Close_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::shared_ptr<Link> link = std::static_pointer_cast<Link>(session->link);
		assert(nullptr != link);

		LOG(DEV, "[link_key:", link->link_key, "] session_key:", (nullptr != link->session) ? link->session->session_key : 0);
		session->handover_safe = false;
		link->strand.wrap(std::bind(&Network::Tcp::LinkManager<SESSION_T>::OnClose, link->link_manager, link, ErrorCode::Success));

		std::shared_ptr<Packet> sendPacket = Packet::Create();
		if (nullptr == sendPacket)
		{
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPacketError))();
			return;
		}

		sendPacket->Write(MSG_ID::MsgID_SvrCli_Close_Ans, nullptr, 0);
		link->AsyncSend(sendPacket);
	}
};
}}}
#endif
