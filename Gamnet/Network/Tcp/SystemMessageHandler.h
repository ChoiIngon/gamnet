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
		MsgID_CliSvr_HeartBeat_Ntf = MsgID_Max - 3,
		MsgID_SvrCli_HeartBeat_Ntf = MsgID_Max - 3,
		MsgID_CliSvr_ReliableAck_Ntf = MsgID_Max - 4,
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
		Json::Value ans;
		ans["error_code"] = 0;
		ans["session_key"] = 0;
		ans["session_token"] = "";

		try
		{
			session->OnCreate();
            session->session_token = Session::GenerateSessionToken(session->session_key);
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

        SendMsg(session, MSG_ID::MsgID_SvrCli_Connect_Ans, ans);
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

            LOG(DEV, "Recv_Reconnect_Req(session_key:", session_key, ", session_token:", session_token, ")");

            if (session_key == session->session_key) // 자기가 자기한테 Reconnect 메시지 어뷰징
            {
                throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionKeyError, "invalid session key(session_key:", session->session_key, ", state:", (int)session->session_state, ")");
            }

            if (Network::Tcp::Session::State::Invalid != session->session_state) // 리컨넥트용 세션이 아니라 뭔가 다른 놈이 어뷰징
            {
                throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "invalid session state(session_key:", session->session_key, ", state:", (int)session->session_state, ")");
            }

			const std::shared_ptr<SESSION_T> prevSession = std::static_pointer_cast<SESSION_T>(session->session_manager->Find(session_key));
			if (nullptr == prevSession) // 재 연결한 세션 못찾음
			{
                throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionKeyError, "[Gamnet::Network::Tcp] can not find session data for reconnect(session_key:", session_key, ")");
			}

            std::shared_ptr<boost::asio::ip::tcp::socket> socket = session->socket;

            prevSession->Dispatch([=]() {
                Json::Value ans;
                ans["error_code"] = 0;

                try
                {
                    if (session_token != prevSession->session_token)
                    {
                        throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionTokenError, "invalid session token(session_key:", prevSession->session_key, ", expect:", prevSession->session_token, ", receive:", session_token, ")");
                    }

                    if(nullptr != prevSession->socket)
                    {
                        throw GAMNET_EXCEPTION(ErrorCode::InvalidSocketError, "prev_session alreay has socket(session_key:", prevSession->session_key, ")");
                    }

                    if(Network::Tcp::Session::State::AfterCreate != prevSession->session_state)
                    {
                        throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionStateError, "invalid session state(session_key:", prevSession->session_key, ", state:", (int)prevSession->session_state, ")");
                    }

                    if(false == prevSession->handover_safe)
                    {
                        throw GAMNET_EXCEPTION(ErrorCode::ReconnectFailError, "not handover_safe session(session_key:", prevSession->session_key, ", handover_safe:", prevSession->handover_safe, ")");
                    }

				    prevSession->socket = socket;
                    prevSession->expire_timer.Cancel();
                    prevSession->send_buffers.clear();

                    SendMsg(prevSession, MSG_ID::MsgID_SvrCli_Reconnect_Ans, ans);

                    for (const std::shared_ptr<Packet>& sendPacket : prevSession->send_packets)
                    {
                        prevSession->Network::Session::AsyncSend(sendPacket);
                    }

                    prevSession->OnAccept();
                    prevSession->session_state = Network::Tcp::Session::State::AfterAccept;
                    prevSession->AsyncRead();
                    return;
                }
                catch (const Exception& e)
                {
                    LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
                    ans["error_code"] = e.error_code();
                }

                SendMsg(session, MSG_ID::MsgID_SvrCli_Reconnect_Ans, ans);
                session->Close(ErrorCode::InvalidSessionError);
			});

            session->socket = nullptr;
            return;
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			ans["error_code"] = e.error_code();
		}

        SendMsg(session, MSG_ID::MsgID_SvrCli_Reconnect_Ans, ans);
		session->Close(ErrorCode::InvalidSessionError);
	}

	void Recv_HeartBeat_Ntf(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
	{
        Json::Value ntf;
        std::string json = std::string(packet->ReadPtr() + Packet::HEADER_SIZE, packet->Size());
        Json::Reader reader;

        if (false == reader.parse(json, ntf))
        {
            throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[Gamnet::Network::Tcp] message format error(data:", json, ")");
        }

        time_t timestamp = ntf["timestamp"].asInt64();

        //SendMsg(session, MSG_ID::MsgID_SvrCli_HeartBeat_Ntf, ans);
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
		session->handover_safe = false;

		std::shared_ptr<Packet> ansPacket = Packet::Create();
		if (nullptr == ansPacket)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "[Gamnet::Network::Tcp] can not create Packet instance");
		}

		ansPacket->Write(MSG_ID::MsgID_SvrCli_Close_Ans, nullptr, 0);
		session->AsyncSend(ansPacket);
	}

    void SendMsg(const std::shared_ptr<SESSION_T>& session, MSG_ID msgID, const Json::Value& msg)
    {
        Json::FastWriter writer;
        std::string str = writer.write(msg);

        std::shared_ptr<Packet> packet = Packet::Create();
        if (nullptr == packet)
        {
            LOG(ERR, "can not create Packet instance");
            return;
        }

        packet->Write(msgID, str.c_str(), str.length());
        session->AsyncSend(packet);
    }
};
}}}
#endif
