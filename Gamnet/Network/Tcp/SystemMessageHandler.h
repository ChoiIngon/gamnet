#ifndef _GAMNET_NETWORK_TCP_SYSTEM_MESSAGE_HANDLER_H_
#define _GAMNET_NETWORK_TCP_SYSTEM_MESSAGE_HANDLER_H_

#include "../../Library/Json/json.h"
#include "../../Library/Singleton.h"
#include "../SessionManager.h"
#include "../Handler.h"
#include "Packet.h"
#include "Session.h"
#undef max

namespace Gamnet {	namespace Network {		namespace Tcp {
	enum MSG_ID {
		MsgID_Max = std::numeric_limits<uint32_t>::max(),
		MsgID_CliSvr_Connect_Req = MsgID_Max - 1,
		MsgID_SvrCli_Connect_Ans = MsgID_Max - 1,
		MsgID_CliSvr_Reconnect_Req = MsgID_Max - 2,
		MsgID_SvrCli_Reconnect_Ans = MsgID_Max - 2,
		MsgID_CliSvr_HeartBeat_Req = MsgID_Max - 3,
		MsgID_SvrCli_HeartBeat_Ans = MsgID_Max - 3,
		MsgID_CliSvr_ReliableAck_Ntf = MsgID_Max - 4,
		MsgID_SvrCli_ReliableAck_Ntf = MsgID_Max - 4,
		MsgID_SvrCli_Kickout_Ntf = MsgID_Max - 5,
		MsgID_CliSvr_Close_Req = MsgID_Max - 6,
		MsgID_SvrCli_Close_Ans = MsgID_Max - 6
	};

template <class SESSION_T>
class SystemMessageHandler : public IHandler
{
public:
	void Recv_Connect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		// LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session->session_key, "] Recv_Connect_Req");
		Json::Value ans;
		ans["error_code"] = 0;
		ans["session_key"] = 0;
		ans["session_token"] = "";

		try
		{
			session->OnCreate();
			session->session_state = Network::Tcp::Session::State::AfterCreate;

            session->handover_safe = true;

			session->OnAccept();
			session->session_state = Network::Tcp::Session::State::AfterAccept;
			session->session_manager->Add(session);

			ans["session_key"] = session->session_key;
			ans["session_token"] = session->session_token;
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

		std::shared_ptr<Packet> ansPacket = Packet::Create();
		if (nullptr == ansPacket)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "[Gamnet::Network::Tcp] can not create Packet instance");
		}

		Json::FastWriter writer;
		std::string str = writer.write(ans);

		ansPacket->Write(MSG_ID::MsgID_SvrCli_Connect_Ans, str.c_str(), str.length());
		session->AsyncSend(ansPacket);
	}

	void Recv_Reconnect_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		Json::Value req;
		Json::Value ans;
		ans["error_code"] = 0;
		try
		{
			std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
			Json::Reader reader;

			if (false == reader.parse(json, req))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[Gamnet::Network::Tcp] message format error(data:", json, ")");
			}

			uint32_t session_key = req["session_key"].asUInt();
			const std::string session_token = req["session_token"].asString();

			LOG(DEV, "[Gamnet::Network::Tcp] Recv_Reconnect_Req(session_key:", session_key, ", session_token:", session_token, ")");
			const std::shared_ptr<SESSION_T> prevSession = std::static_pointer_cast<SESSION_T>(session->session_manager->Find(session_key));
			if (nullptr == prevSession)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionKeyError, "[Gamnet::Network::Tcp] can not find session data for reconnect(session_key:", session_key, ")");
			}

			prevSession->Dispatch([=]() {
                if (session_token != prevSession->session_token)
                {
                    throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionTokenError, "[Gamnet::Network::Tcp] invalid session token(expect:", prevSession->session_token, ", receive:", session_token, ")");
                }

                if(Network::Tcp::Session::State::AfterCreate != prevSession->session_state || false == prevSession->handover_safe)
                {
                    throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionStateError, "[Gamnet::Network::Tcp] invalid session state(state:", (int)prevSession->session_state, ", handover_safe:", prevSession->handover_safe, ")");
                }

                std::shared_ptr<Packet> ansPacket = Packet::Create();
                if (nullptr == ansPacket)
                {
                    throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "[Gamnet::Network::Tcp] can not create packet)");
                }

                std::shared_ptr<boost::asio::ip::tcp::socket> socket = session->socket;
                session->socket = nullptr;

				prevSession->socket = socket;
                prevSession->expire_timer.Cancel();

				Json::FastWriter writer;
				std::string str = writer.write(ans);

				ansPacket->Write(MSG_ID::MsgID_SvrCli_Reconnect_Ans, str.c_str(), str.length());
				prevSession->send_buffers.clear();
				prevSession->AsyncSend(ansPacket);
				prevSession->OnAccept();
				prevSession->session_state = Network::Tcp::Session::State::AfterAccept;
				for (const std::shared_ptr<Packet>& sendPacket : prevSession->send_packets)
				{
					prevSession->Network::Session::AsyncSend(sendPacket);
				}
				prevSession->AsyncRead();
			});

            return;
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

		//LOG(DEV, "[link_key:", link->link_key, "] error_code:", ans["error_code"].asUInt());
		Json::FastWriter writer;
		std::string str = writer.write(ans);

		std::shared_ptr<Packet> ansPacket = Packet::Create();
		if (nullptr == ansPacket)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "[Gamnet::Network::Tcp] can not create Packet instance");
		}

		ansPacket->Write(MSG_ID::MsgID_SvrCli_Reconnect_Ans, str.c_str(), str.length());
		session->AsyncSend(ansPacket);
		session->session_token = "";
		session->Close(ErrorCode::InvalidSessionError);
	}

	void Recv_HeartBeat_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::shared_ptr<Packet> ansPacket = Packet::Create();
		if (nullptr == ansPacket)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
		}

		Json::Value ans;
		ans["error_code"] = 0;
		ans["msg_seq"] = session->recv_seq;

		Json::FastWriter writer;
		std::string str = writer.write(ans);

		ansPacket->Write(MSG_ID::MsgID_SvrCli_HeartBeat_Ans, str.c_str(), str.length());
		session->AsyncSend(ansPacket);
	}

	void Recv_ReliableAck_Ntf(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
		Json::Value ntf;
		Json::Reader reader;
		if (false == reader.parse(json, ntf))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[Gamnet::Network::Tcp] message format error(data:", json, ")");
		}

		uint32_t ackSEQ = ntf["ack_seq"].asUInt();

		while (0 < session->send_packets.size())
		{
			const std::shared_ptr<Packet>& ansPacket = session->send_packets.front();
			if (ackSEQ < ansPacket->msg_seq)
			{
				break;
			}
			session->send_packets.pop_front();
		}
	}

	void Recv_Close_Req(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
		//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", link->session->session_key , "] Recv_Close_Req");

		session->handover_safe = false;

		std::shared_ptr<Packet> ansPacket = Packet::Create();
		if (nullptr == ansPacket)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "[Gamnet::Network::Tcp] can not create Packet instance");
		}

		ansPacket->Write(MSG_ID::MsgID_SvrCli_Close_Ans, nullptr, 0);
		session->AsyncSend(ansPacket);
	}
};
}}}
#endif
