#ifndef GAMNET_TEST_SESSION_MANAGER_H_
#define GAMNET_TEST_SESSION_MANAGER_H_

#include "../Network/Tcp/SessionManager.h"
#include "../Network/Connector.h"
#include "Session.h"
#include "TestHandler.h"

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
		
		struct TestExecuteInfo
		{
			TestExecuteInfo() : name(""), execute_count(0), fail_count(0), elapse_time(0) {
			}
			std::string			name;
			std::atomic<int64_t>	execute_count;
			std::atomic<int64_t>	fail_count;
			uint64_t				elapse_time;
			SEND_HANDLER_TYPE		send_handler;
			std::map<uint32_t, RECV_HANDLER_TYPE> recv_handlers;
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
		TestHandler<SESSION_T> test_handler;
		std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;
		std::string host;
		int			port;
	public:
		SessionManager();
		virtual ~SessionManager();

		void Init(const char* host, int port, int session_count, int loop_count);
		void Run();

		virtual std::shared_ptr<Network::Session> OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override;

		virtual void OnDestroy(uint32_t sessionKey) override
		{
			BASE_T::OnDestroy(sessionKey);
			finish_execute_count += 1;
			if (max_execute_count > begin_execute_count)
			{
				begin_execute_count++;
				connector.AsyncConnect(host.c_str(), port, 0);
			}
		}
		
		virtual void OnReceive(const std::shared_ptr<Network::Session>& s, const std::shared_ptr<Buffer>& buffer) override
		{
			const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(s);
			const std::shared_ptr<Network::Tcp::Packet>& packet = std::static_pointer_cast<Network::Tcp::Packet>(buffer);
			
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
				catch (const Exception& /*e*/)
				{
					execute_info->fail_count++;
				}
				
				if (session->recv_seq < packet->msg_seq)
				{
					session->recv_seq = packet->msg_seq;
				}

				if (true == packet->reliable)
				{
					this->test_handler.Send_ReliableAck_Ntf(session);
				}

				if (false == session->is_pause)
				{
					ExecuteSendHandler(session);
				}
			}
			session->socket = nullptr;
		}
		
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
	};

	template <class SESSION_T>
	SessionManager<SESSION_T>::SessionManager() : begin_execute_count(0), finish_execute_count(0), max_execute_count(0), session_count(0), connector(this), host(""), port(0)
	{
		BindSendHandler("__connect__", std::bind(&TestHandler<SESSION_T>::Send_Connect_Req, &test_handler, std::placeholders::_1));
		BindRecvHandler("__connect__", (uint32_t)Network::Tcp::MsgID_SvrCli_Connect_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Connect_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2));
		BindGlobalRecvHandler((uint32_t)Network::Tcp::MsgID_SvrCli_Reconnect_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Reconnect_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2));
		BindSendHandler("__close__", std::bind(&TestHandler<SESSION_T>::Send_Close_Req, &test_handler, std::placeholders::_1));
		BindRecvHandler("__close__", (uint32_t)Network::Tcp::MsgID_SvrCli_Close_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Close_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2));
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
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindSendHandler(const std::string& handlerName, typename SessionManager<SESSION_T>::SEND_HANDLER_TYPE sendHandler)
	{
		std::shared_ptr<TestExecuteInfo> executeInfo = std::make_shared<TestExecuteInfo>();
		executeInfo->name = handlerName;
		executeInfo->send_handler = sendHandler;
		if (false == execute_infos.insert(std::make_pair(handlerName, executeInfo)).second)
		{
			GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "(duplicate test case name(", handlerName, ")");
		}
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindRecvHandler(const std::string& handlerName, uint32_t msgID, typename SessionManager<SESSION_T>::RECV_HANDLER_TYPE recv)
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
		session->execute_send_handler = std::bind(&SessionManager<SESSION_T>::ExecuteSendHandler, this, std::placeholders::_1);
		try {
			if (0 == execute_order.size())
			{
				return session;
			}

			session->send_time = std::chrono::steady_clock::now();
			if (0 == session->test_seq)
			{
				const std::shared_ptr<TestExecuteInfo>& info = this->execute_order[0];
				info->execute_count++;
				test_handler.Send_Connect_Req(session);
			}
			else
			{
				test_handler.Send_Reconnect_Req(session);
			}
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		}

		return session;
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
}} /* namespace Gamnet */

#endif /* TEST_H_ */
