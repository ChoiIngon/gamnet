#ifndef GAMNET_TEST_SESSION_MANAGER_H_
#define GAMNET_TEST_SESSION_MANAGER_H_

#include "../Network/Tcp/SessionManager.h"
#include "../Network/Connector.h"
#include "Config.h"
#include "Executer.h"
#include "Session.h"

#include <map>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>

namespace Gamnet {	namespace Test { 
	template <class SESSION_T>
	class SessionManager : public Network::Tcp::SessionManager<SESSION_T>
	{
		GAMNET_WHERE(SESSION_T, Session);
	private:
		typedef Network::Tcp::SessionManager<SESSION_T> BASE_T;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
		
		struct ReceiveHandlerInfo
		{
			std::string name;
			RECV_HANDLER_TYPE receive_handler;
		};

		struct TestExecuteInfo
		{
			TestExecuteInfo() : name(""), execute_count(0), fail_count(0), elapse_time(0) {
			}
			std::string				name;
			std::atomic<int64_t>	execute_count;
			std::atomic<int64_t>	fail_count;
			uint64_t				elapse_time;
			SEND_HANDLER_TYPE		send_handler;
			std::map<uint32_t, ReceiveHandlerInfo> receive_handlers;
		};

		std::map<std::string, std::shared_ptr<TestExecuteInfo>>	execute_infos;
		std::map<uint32_t, RECV_HANDLER_TYPE> 	global_recv_handlers;
			
		Log::Logger				log;
		Time::Timer 			log_timer;
		
		std::atomic<int64_t>	begin_execute_count;
		std::atomic<int64_t>	finish_execute_count;
		int 					max_execute_count;
		int						session_count;
		Network::Connector		connector;
		
		std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;
		std::string host;
		int			port;

		Config config;
	public:
		SessionManager();
		virtual ~SessionManager();

		void Init(const char* host, int port, int session_count, int loop_count);
		void Run();

		virtual std::shared_ptr<Network::Session> OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override;

		virtual void OnDestroy(uint32_t sessionKey) override;
		virtual void OnReceive(const std::shared_ptr<Network::Session>& s, const std::shared_ptr<Buffer>& buffer) override;
		
		void ExecuteSendHandler(const std::shared_ptr<Session>& session)
		{
			if ((int)this->execute_order.size() <= session->test_seq)
			{
				session->socket = nullptr;
				return;
			}
			
			const std::shared_ptr<TestExecuteInfo>& executeInfo = this->execute_order[session->test_seq];
			session->send_time = std::chrono::steady_clock::now();
			executeInfo->send_handler(std::static_pointer_cast<SESSION_T>(session));
			executeInfo->execute_count++;
		}

		void BindSendHandler(const std::string& handlerName, SEND_HANDLER_TYPE sendHandler);
		void BindRecvHandler(const std::string& test_name, uint32_t msgID, RECV_HANDLER_TYPE recv);
		void BindGlobalRecvHandler(uint32_t msgID, RECV_HANDLER_TYPE recv);
		
		void RegisterTestcase(const std::string& test_name, bool tail = true)
		{
			auto itr_execute_info = execute_infos.find(test_name);
			if (execute_infos.end() == itr_execute_info)
			{
#ifdef _WIN32
				MessageBoxA(nullptr, Format("can't find registered test case execute info(test_name:", test_name, ")").c_str(), "No Test Execute Info", MB_ICONWARNING);
#endif
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", test_name, ")");
			}

			std::shared_ptr<TestExecuteInfo> info = itr_execute_info->second;
			if (true == tail)
			{
				execute_order.push_back(info);
			}
			else
			{
				execute_order.insert(execute_order.begin(), info);
			}
		}
		
	private:
		void OnLogTimerExpire()
		{
			//log.Write(GAMNET_INF, "[Test] link count..(active:", this->Size(), ", available:", link_pool.Available(), ", max:", link_pool.Capacity(), ")");
			//log.Write(GAMNET_INF, "[Test] session count..(active:", this->session_manager.Size(), ", available:", this->session_pool.Available(), ", max:", this->session_pool.Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] begin count..(", begin_execute_count, "/", max_execute_count, ")");

			for (auto itr : execute_order)
			{
				if (0 < itr->execute_count)
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", execute:", itr->execute_count, ", fail:", itr->fail_count, ", total_time:", itr->elapse_time, "ms, average_time:", itr->elapse_time / itr->execute_count, "ms)");
				}
				else
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", execute:", itr->execute_count, ", fail:0, total_time:0ms, average_time:0ms)");
				}
			}
			log.Write(GAMNET_INF, "[Test] ==============================================================");

			if (finish_execute_count >= max_execute_count)
			{
				log_timer.Cancel();
				log.Write(GAMNET_INF, "[Test] test finished..(", finish_execute_count, "/", max_execute_count, ")");
			}
		}

		void Send_Connect_Req(const std::shared_ptr<SESSION_T>& session);
		void Recv_Connect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
		void Send_Reconnect_Req(const std::shared_ptr<SESSION_T>& session);
		void Recv_Reconnect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
		void Send_ReliableAck_Ntf(const std::shared_ptr<SESSION_T>& session);
		void Send_Close_Req(const std::shared_ptr<SESSION_T>& session);
		void Recv_Close_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
	};

	template <class SESSION_T>
	SessionManager<SESSION_T>::SessionManager() : begin_execute_count(0), finish_execute_count(0), max_execute_count(0), session_count(0), connector(this), host(""), port(0)
	{
		BindSendHandler("__connect__", std::bind(&SessionManager<SESSION_T>::Send_Connect_Req, this, std::placeholders::_1));
		BindRecvHandler("__connect__", (uint32_t)Network::Tcp::MsgID_SvrCli_Connect_Ans, std::bind(&SessionManager<SESSION_T>::Recv_Connect_Ans, this, std::placeholders::_1, std::placeholders::_2));
		BindGlobalRecvHandler((uint32_t)Network::Tcp::MsgID_SvrCli_Reconnect_Ans, std::bind(&SessionManager<SESSION_T>::Recv_Reconnect_Ans, this, std::placeholders::_1, std::placeholders::_2));
		BindSendHandler("__close__", std::bind(&SessionManager<SESSION_T>::Send_Close_Req, this, std::placeholders::_1));
		BindRecvHandler("__close__", (uint32_t)Network::Tcp::MsgID_SvrCli_Close_Ans, std::bind(&SessionManager<SESSION_T>::Recv_Close_Ans, this, std::placeholders::_1, std::placeholders::_2));
	}

	template <class SESSION_T>
	SessionManager<SESSION_T>::~SessionManager()
	{
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Init(const char* host, int port, int session_count, int loop_count)
	{
		RegisterTestcase("__connect__", false);
		RegisterTestcase("__close__", true);

		log.Init("test", "test", 5);
		if (0 == session_count)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidArgumentError, " 'session_count' should be set");
		}

		this->host = host;
		this->port = port;
		this->begin_execute_count = 0;
		this->finish_execute_count = 0;
		this->max_execute_count = session_count * loop_count;
		this->session_count = session_count;

		config.ReadXml("config.xml");
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindSendHandler(const std::string& messageName, typename SessionManager<SESSION_T>::SEND_HANDLER_TYPE sendHandler)
	{
		std::shared_ptr<TestExecuteInfo> executeInfo = std::make_shared<TestExecuteInfo>();
		executeInfo->name = messageName;
		executeInfo->send_handler = sendHandler;
		if (false == execute_infos.insert(std::make_pair(handlerName, executeInfo)).second)
		{
			GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "(duplicate test case name(", handlerName, ")");
		}
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindRecvHandler(const std::string& messageName, uint32_t msgID, typename SessionManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		auto itr = execute_infos.find(handlerName);
		if (execute_infos.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", handlerName, ")");
		}
		std::shared_ptr<TestExecuteInfo> executeInfo = itr->second;
		if (false == executeInfo->recv_handlers.insert(std::make_pair(msgID, recv)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "duplicated recv msg id(test_name:", handlerName, ", msg_id:", msgID, ")");
		}
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindGlobalRecvHandler(uint32_t msgID, typename SessionManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		if (false == global_recv_handlers.insert(std::make_pair(msgID, recv)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "duplicated global recv msg id(msg_id:", msgID, ")");
		}
	}

	template <class SESSION_T>
	std::shared_ptr<Network::Session> SessionManager<SESSION_T>::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(BASE_T::OnConnect(socket));
		if("" == session->session_token)
		{
			Send_Connect_Req(session);
		}
		else
		{
			Send_Reconnect_Req(session);
		}

		return session;
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Send_Connect_Req(const std::shared_ptr<SESSION_T>& session)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if (nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::CreateInstanceFailError, "can not create packet");
		}
		packet->Write(Network::Tcp::MsgID_CliSvr_Connect_Req, nullptr, 0);
		session->AsyncSend(packet);
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Recv_Connect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
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

		Singleton<Executer>::GetInstance().OnCondition("OnConnect", {}, session);
		session->Next();
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Send_Reconnect_Req(const std::shared_ptr<SESSION_T>& session)
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

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Recv_Reconnect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
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
		Singleton<Executer>::GetInstance().OnCondition("OnReconnect", {}, session);
		//session->Resume();
		//session->Next();
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Send_ReliableAck_Ntf(const std::shared_ptr<SESSION_T>& session)
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

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Send_Close_Req(const std::shared_ptr<SESSION_T>& session)
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

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Recv_Close_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		session->Close(0);
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::OnReceive(const std::shared_ptr<Network::Session>& s, const std::shared_ptr<Buffer>& buffer)
	{
		const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(s);
		const std::shared_ptr<Network::Tcp::Packet>& packet = std::static_pointer_cast<Network::Tcp::Packet>(buffer);
		const std::string message = "";
		Singleton<Executer>::GetInstance().OnCondition("OnReceive", { { "message", message } }, session);
		/*
		if (session->test_seq < (int)this->execute_order.size())
		{
			const std::shared_ptr<TestExecuteInfo> execute_info = this->execute_order[session->test_seq];
			auto itr = execute_info->recv_handlers.find(packet->msg_id);
			if (execute_info->recv_handlers.end() != itr)
			{
				execute_info->elapse_time += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - session->send_time).count();
			}
			else
			{
				itr = global_recv_handlers.find(packet->msg_id);
				if (global_recv_handlers.end() == itr)
				{
					throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't find handler function(msg_id:", packet->msg_id, ")");
				}
			}

			RECV_HANDLER_TYPE& handler = itr->second;
			session->is_pause = true;
			try {
				handler(session, packet);
			}
			catch (const Exception& e)
			{
				execute_info->fail_count++;
			}

			if (session->recv_seq < packet->msg_seq)
			{
				session->recv_seq = packet->msg_seq;
			}

			if (true == packet->reliable)
			{
				Send_ReliableAck_Ntf(session);
			}

			if (false == session->is_pause)
			{
				ExecuteSendHandler(session);
			}
		}
		session->socket = nullptr;
		*/
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::Run()
	{
		log_timer.AutoReset(true);
		log_timer.SetTimer(3000, std::bind(&SessionManager<SESSION_T>::OnLogTimerExpire, this));

		for (size_t i = 0; i < this->session_count; i++)
		{
			begin_execute_count++;
			if (max_execute_count < begin_execute_count)
			{
				break;
			}
			connector.AsyncConnect(host.c_str(), port, 0);
		}
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::OnDestroy(uint32_t sessionKey) 
	{
		SessionManager<SESSION_T>::BASE_T::OnDestroy(sessionKey);
		finish_execute_count += 1;
		if (max_execute_count > begin_execute_count)
		{
			begin_execute_count++;
			connector.AsyncConnect(host.c_str(), port, 0);
		}
	}
}} /* namespace Gamnet */

#endif /* TEST_H_ */
