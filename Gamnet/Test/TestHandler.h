#ifndef _GAMNET_TEST_TESTHANDLER_H_
#define _GAMNET_TEST_TESTHANDLER_H_

#include "../Library/Json/json.h"
#include "../Network/Network.h"

namespace Gamnet { namespace Test {
	template <class SESSION_T>
	class TestHandler
	{
	public :
		void Send_Connect_Req(const std::shared_ptr<SESSION_T>& session)
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
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] parse error(msg:", json, ")");
			}

			if (ErrorCode::Success != ans["error_code"].asInt())
			{
				session->link->strand.wrap(std::bind(&Network::Link::Close, session->link, ans["error_code"].asInt()))();
				return;
			}

			session->server_session_key = ans["session_key"].asUInt();
			session->session_key = session->server_session_key;
			session->server_session_token = ans["session_token"].asString();
			session->is_connected = true;
			session->OnConnect();
		}

		void Send_Reconnect_Req(const std::shared_ptr<SESSION_T>& session)
		{
			//LOG(DEV, "[session_key:", session->session_key, "]");
			std::shared_ptr<Network::Tcp::Link> link = std::static_pointer_cast<Network::Tcp::Link>(session->link);
			if (nullptr == link)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "invalid link(session_key:", session->session_key, ")");
			}

			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if (nullptr == packet)
			{
				throw GAMNET_EXCEPTION(ErrorCode::CreateInstanceFailError, "can not create packet");
			}

			Json::Value req;
			req["session_key"] = session->server_session_key;
			req["session_token"] = session->server_session_token;

			Json::FastWriter writer;
			std::string str = writer.write(req);

			packet->Write(Network::Tcp::MsgID_CliSvr_Reconnect_Req, str.c_str(), str.length());
			session->AsyncSend(packet);
		}

		void Recv_Reconnect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			std::string json = std::string(packet->ReadPtr() + Network::Tcp::Packet::HEADER_SIZE, packet->Size());
			Json::Value ans;
			Json::Reader reader;
			if (false == reader.parse(json, ans))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "[link_key:", session->link->link_key, ", session_key:", session->session_key, "] parse error(msg:", json, ")");
			}

			if (ErrorCode::Success != ans["error_code"].asInt())
			{
				session->link->strand.wrap(std::bind(&Network::Link::Close, session->link, ans["error_code"].asInt()))();
				return;
			}

			session->is_connected = true;
			session->OnConnect();
			session->Resume();
		}

		void Send_ReliableAck_Ntf(const std::shared_ptr<SESSION_T>& session)
		{
			std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
			if (nullptr == packet)
			{
				LOG(ERR, "can not create packet");
				session->link->strand.wrap(std::bind(&Network::Link::Close, session->link, ErrorCode::NullPacketError))();
				return;
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
				LOG(GAMNET_ERR, "can not create packet");
				return;
			}
			if (false == packet->Write(Network::Tcp::MsgID_CliSvr_Close_Req, nullptr, 0))
			{
				LOG(GAMNET_ERR, "fail to serialize packet");
				return;
			}
			session->AsyncSend(packet);
		}

		void Recv_Close_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			std::shared_ptr<Network::Link> link = session->link;
			if(nullptr == link)
			{
				return;
			}	

			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::Success))();
		}
	};
}}
#endif
