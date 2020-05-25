#include "SessionManager.h"
#include "../Network/Tcp/SystemMessageHandler.h"

namespace Gamnet { namespace Test {
	void SessionManagerImpl::Init(const char* host_, int port_, int sessionCount_, int loopCount_)
	{
		log_timer = Time::Timer::Create();
		log_timer->AutoReset(true);
		log_timer->SetTimer(5000, std::bind(&SessionManagerImpl::OnLogTimerExpire, this));

		log.Init("test", "test", 5);
		if (0 == sessionCount_)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidArgumentError, " 'session_count' should be set");
		}

		host = host_;
		port = port_;
		begin_execute_count = 0;
		finish_execute_count = 0;
		session_count = sessionCount_;
		max_execute_count = sessionCount_ * loopCount_;
	}

	void SessionManagerImpl::Send_Connect_Req(const std::shared_ptr<Session>& session)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if (nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CreateInstanceFailError, "can not create packet");
		}
		packet->Write(Network::Tcp::MsgID_CliSvr_Connect_Req, nullptr, 0);
		session->AsyncSend(packet);
	}

	void SessionManagerImpl::Recv_Connect_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
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

	void SessionManagerImpl::Send_Reconnect_Req(const std::shared_ptr<Session>& session)
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

	void SessionManagerImpl::Recv_Reconnect_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
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

		for (const std::shared_ptr<Network::Tcp::Packet>& sendPacket : session->send_packets)
		{
			session->AsyncSend(sendPacket);
		}
		session->OnConnect();
		//session->Resume();
		//session->Next();
	}

	void SessionManagerImpl::Send_ReliableAck_Ntf(const std::shared_ptr<Session>& session)
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

	void SessionManagerImpl::Send_Close_Req(const std::shared_ptr<Session>& session)
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

	void SessionManagerImpl::Recv_Close_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		session->Close(0);
	}


	void SessionManagerImpl::OnLogTimerExpire()
	{
		//log.Write(GAMNET_INF, "[Test] link count..(active:", this->Size(), ", available:", link_pool.Available(), ", max:", link_pool.Capacity(), ")");
		//log.Write(GAMNET_INF, "[Test] session count..(active:", this->session_manager.Size(), ", available:", this->session_pool.Available(), ", max:", this->session_pool.Capacity(), ")");
		log.Write(GAMNET_INF, "[Test] begin count..(", begin_execute_count, "/", max_execute_count, ")");

		if (finish_execute_count >= max_execute_count)
		{
			log_timer->Cancel();
			log.Write(GAMNET_INF, "[Test] test finished..(", finish_execute_count, "/", max_execute_count, ")");
		}
	}
}}