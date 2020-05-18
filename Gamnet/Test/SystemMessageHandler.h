#ifndef _GAMNET_TEST_SYSTEM_MESSAGE_HANDLER_H_
#define _GAMNET_TEST_SYSTEM_MESSAGE_HANDLER_H_

#include "../Library/Json/json.h"
#include "../Network/Tcp/Packet.h"
#include "../Network/Tcp/SystemMessageHandler.h"
#include "Session.h"

namespace Gamnet { namespace Test {
	class SessionManager* const session_manager;
	template <class SESSION_T>
	class SystemMessageHandler
	{
	public :
		SystemMessageHandler(SessionManager* const session_manager);
		~SystemMessageHandler();

		void Send_Connect_Req(const std::shared_ptr<Session>& session)
		{
			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if (nullptr == packet)
			{
				throw GAMNET_EXCEPTION(ErrorCode::CreateInstanceFailError, "can not create packet");
			}
			packet->Write(Network::Tcp::MsgID_CliSvr_Connect_Req, nullptr, 0);
			session->AsyncSend(packet);
		}

		void Recv_Connect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			std::string json = std::string(packet->ReadPtr() + Network::Tcp::Packet::HEADER_SIZE, packet->Size());
			Json::Value ans;
			Json::Reader reader;
			if (false == reader.parse(json, ans))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[Gamnet::Test] parse error(msg:", json, ")");
			}

			if (ErrorCode::Success != ans["error_code"].asInt())
			{
				throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[Gamnet::Test] connect fail(error_code:", ans["error_code"].asInt(), ")");
			}

			session->session_key = ans["session_key"].asUInt();
			session->session_token = ans["session_token"].asString();
			session->OnConnect();
			session->Next();
		}

		void Send_Reconnect_Req(const std::shared_ptr<SESSION_T>& session)
		{
			//LOG(DEV, "[", link->link_manager->name, "/", link->link_key, "/", session->session_key, "] Send_Reconnect_Req");

			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if (nullptr == packet)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
			}

			Json::Value req;
			req["session_key"] = session->session_key;
			req["session_token"] = session->session_token;

			Json::FastWriter writer;
			std::string str = writer.write(req);

			packet->Write(Network::Tcp::MsgID_CliSvr_Reconnect_Req, str.c_str(), str.length());
			session->AsyncSend(packet);
		}

		void Recv_Reconnect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			//LOG(DEV, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Recv_Reconnect_Ans");
			std::string json = std::string(packet->ReadPtr() + Network::Tcp::Packet::HEADER_SIZE, packet->Size());
			Json::Value ans;
			Json::Reader reader;
			if (false == reader.parse(json, ans))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "parse error(msg:", json, ")");
			}

			if (ErrorCode::Success != ans["error_code"].asInt())
			{
				throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[Gamnet::Test] reconnect fail(error_code:", ans["error_code"].asInt(), ")");
			}

			session->is_connected = true;

			for (const std::shared_ptr<Network::Tcp::Packet>& sendPacket : session->send_packets)
			{
				session->AsyncSend(sendPacket);
			}
			session->OnConnect();
			//session->Resume();
			//session->Next();
		}

		void Send_ReliableAck_Ntf(const std::shared_ptr<SESSION_T>& session)
		{
			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if (nullptr == packet)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
			}

			Json::Value ntf;
			ntf["ack_seq"] = session->recv_seq;

			Json::FastWriter writer;
			std::string str = writer.write(ntf);

			packet->Write(Network::Tcp::MsgID_CliSvr_ReliableAck_Ntf, str.c_str(), str.length());
			session->AsyncSend(packet);
			//LOG(DEV, "[link_key:", link->link_key, "]");
		}

		void Send_Close_Req(const std::shared_ptr<SESSION_T>& session)
		{
			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if (nullptr == packet)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
			}
			if (false == packet->Write(Network::Tcp::MsgID_CliSvr_Close_Req, nullptr, 0))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize packet");
			}

			//LOG(DEV, "[", session->link->link_manager->name, "::link_key:", session->link->link_key, "]");
			session->AsyncSend(packet);
		}

		void Recv_Close_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			//std::shared_ptr<Network::Link> link = session->link;
			//assert(nullptr != link);
			////LOG(DEV, "[", link->link_manager->name, "/", link->link_key, "/", session->session_key, "] Recv_Close_Ans");
			//link->Close(ErrorCode::Success);
		}
	};

#include "SessionManager.h"
	template <class SESSION_T>
	SystemMessageHandler<SESSION_T>::SystemMessageHandler(SessionManager* const session_manager) : session_manager(session_manager)
	{
	}
	template <class SESSION_T>
	SystemMessageHandler<SESSION_T>::~SystemMessageHandler()
	{
	}
}}
#endif
