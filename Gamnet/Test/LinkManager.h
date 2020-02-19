#ifndef GAMNET_TEST_TESTER_H_
#define GAMNET_TEST_TESTER_H_

#include "../Network/Tcp/LinkManager.h"
#include "../Library/ThreadPool.h"
#include "Session.h"
#include "TestHandler.h"

#include <map>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>

namespace Gamnet {	namespace Test {
	template <class SESSION_T>
	class LinkManager : public Network::Tcp::LinkManager<SESSION_T>
	{
		GAMNET_WHERE(SESSION_T, Session);
	public:
		typedef typename Network::Tcp::LinkManager<SESSION_T> BASE_T;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
		
	private:
		struct TestExecuteInfo
		{
			TestExecuteInfo()
			{
				name = "";
				execute_count = 0;
				fail_count = 0;
				elapsed_time = 0;
			}
			std::string			name;
			std::atomic_uint	execute_count;
			std::atomic_uint	fail_count;
			uint64_t			elapsed_time;
			SEND_HANDLER_TYPE	send_handler;
			std::map<uint32_t, RECV_HANDLER_TYPE> recv_handlers;
		};

		std::map<std::string, std::shared_ptr<TestExecuteInfo>>	execute_infos;
		std::map<uint32_t, RECV_HANDLER_TYPE> 	global_recv_handlers;
		Pool<Network::Tcp::Link, std::mutex, Network::Link::InitFunctor, Network::Link::ReleaseFunctor> link_pool;

		Log::Logger		log;
		Timer 			log_timer;
		
		std::atomic_int	begin_execute_count;
		std::atomic_int	finish_execute_count;
		int 			max_execute_count;
		int				session_count;
	public:
		std::string host;
		int			port;

		TestHandler<SESSION_T> test_handler;
		std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;

		LinkManager() : log_timer(GetIOService()), begin_execute_count(0), finish_execute_count(0), max_execute_count(0), link_pool(65535, BASE_T::LinkFactory(this)), host(""), port(0)
		{
			this->name = "Gamnet::Test::LinkManager";

			BindSendHandler("__connect__", std::bind(&TestHandler<SESSION_T>::Send_Connect_Req, &test_handler, std::placeholders::_1));
			BindRecvHandler("__connect__", (uint32_t)Network::Tcp::MsgID_SvrCli_Connect_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Connect_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2));
			BindGlobalRecvHandler((uint32_t)Network::Tcp::MsgID_SvrCli_Reconnect_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Reconnect_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2));

			BindSendHandler("__close__", std::bind(&TestHandler<SESSION_T>::Send_Close_Req, &test_handler, std::placeholders::_1));
			BindRecvHandler("__close__", (uint32_t)Network::Tcp::MsgID_SvrCli_Close_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Close_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2));
		}

		virtual ~LinkManager()
		{
		}

		void Init(const char* host, int port, int session_count, int loop_count)
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
		
		void Run()
		{
			log_timer.AutoReset(true);
			log_timer.SetTimer(3000, std::bind(&LinkManager<SESSION_T>::OnLogTimerExpire, this));
			Test::CreateThreadPool(std::thread::hardware_concurrency());
			for (size_t i = 0; i<this->session_count; i++)
			{
				begin_execute_count++;
				if(max_execute_count < begin_execute_count)
				{
					break;
				}
				Connect();
			}
		}
		
		virtual void OnConnect(const std::shared_ptr<Network::Link>& link) override
		{
			std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(tcpLink->session);
			assert(nullptr != session);

			try {
				if (0 == execute_order.size())
				{
					return;
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
		}

		virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override
		{
			std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(tcpLink->session);
			if (nullptr == session)
			{
				return;
			}

			if(true == session->is_connected)
			{
				session->OnClose(reason);
			}
			session->OnDestroy();
			session->link = nullptr;
			tcpLink->session = nullptr;
			finish_execute_count += 1;
			if (max_execute_count > begin_execute_count)
			{
				begin_execute_count++;
				Connect();
			}
		}

		virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override
		{
			std::shared_ptr<Network::Tcp::Link> tcpLink = std::static_pointer_cast<Network::Tcp::Link>(link);
			const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(tcpLink->session);
			if(nullptr == session)
			{
				return;
			}
			const std::shared_ptr<Network::Tcp::Packet>& packet = std::static_pointer_cast<Network::Tcp::Packet>(buffer);

			if (session->test_seq < (int)this->execute_order.size())
			{
				const std::shared_ptr<TestExecuteInfo> execute_info = this->execute_order[session->test_seq];
				auto itr = execute_info->recv_handlers.find(packet->msg_id);
				if(execute_info->recv_handlers.end() != itr)
				{
					execute_info->elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - session->send_time).count();
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
				if(session->recv_seq < packet->msg_seq)
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
			else
			{
				link->Close(ErrorCode::Success);
			}
		}
		
		void ExecuteSendHandler(const std::shared_ptr<Session>& session)
		{
			if((int)this->execute_order.size() <= session->test_seq)
			{
				session->link->Close(ErrorCode::Success);
				return;
			}
			const std::shared_ptr<TestExecuteInfo>& executeInfo = this->execute_order[session->test_seq];
			session->send_time = std::chrono::steady_clock::now();
			executeInfo->send_handler(std::static_pointer_cast<SESSION_T>(session));
			executeInfo->execute_count++;
		}

		void BindSendHandler(const std::string& test_name, SEND_HANDLER_TYPE send)
		{
			std::shared_ptr<TestExecuteInfo> executeInfo = std::shared_ptr<TestExecuteInfo>(new TestExecuteInfo());
			executeInfo->name = test_name;
			executeInfo->send_handler = send;
			if (false == execute_infos.insert(std::make_pair(test_name, executeInfo)).second)
			{
				GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "(duplicate test case name(", test_name, ")");
			}
		}

		void BindRecvHandler(const std::string& test_name, uint32_t msgID, RECV_HANDLER_TYPE recv)
		{
			auto itr = execute_infos.find(test_name);
			if(execute_infos.end() == itr)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", test_name, ")");
			}
			std::shared_ptr<TestExecuteInfo> executeInfo = itr->second;
			if(false == executeInfo->recv_handlers.insert(std::make_pair(msgID, recv)).second)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "duplicated recv msg id(test_name:", test_name, ", msg_id:", msgID, ")");
			}
		}

		void BindGlobalRecvHandler(uint32_t msgID, RECV_HANDLER_TYPE recv)
		{
			if(false == global_recv_handlers.insert(std::make_pair(msgID, recv)).second)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "duplicated global recv msg id(msg_id:", msgID, ")");
			}
		}

		void RegisterTestcase(const std::string& test_name, bool tail = true)
		{
			auto itr_execute_info = execute_infos.find(test_name);
			if (execute_infos.end() == itr_execute_info)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", test_name, ")");
			}
			
			std::shared_ptr<TestExecuteInfo> info = itr_execute_info->second;
			if(true == tail)
			{
				execute_order.push_back(info);
			}
			else
			{
				execute_order.insert(execute_order.begin(), info);
			}
		}

		void Connect(std::shared_ptr<SESSION_T> session = nullptr)
		{
			std::shared_ptr<Network::Tcp::Link> link = link_pool.Create();
			if (nullptr == link)
			{
				finish_execute_count++;
				LOG(ERR, "can not create 'Test::Link' instance(", link_pool.Available(), "/", link_pool.Capacity(), ")");
				return;
			}

			if(nullptr == session)
			{
				session = session_pool.Create();
				if (nullptr == session)
				{
					finish_execute_count++;
					LOG(ERR, "can not create 'Test::Session' instance");
					return;
				}
			}

			session->link = link;
			link->session = session;
			session->execute_send_handler = std::bind(&LinkManager<SESSION_T>::ExecuteSendHandler, this, std::placeholders::_1);
			session->OnCreate();
			link->Connect(host.c_str(), port, 5);
		}
	private :
		void OnLogTimerExpire()
		{
			log.Write(GAMNET_INF, "[Test] link count..(active:", Size(), ", available:", link_pool.Available(), ", max:", link_pool.Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] session count..(active:", session_manager.Size(), ", available:", session_pool.Available(), ", max:", session_pool.Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] begin count..(", begin_execute_count, "/", max_execute_count, ")");

			for (auto itr : execute_order)
			{
				if (0 < itr->execute_count)
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", execute:", itr->execute_count, ", fail:", itr->fail_count, ", total_time:", itr->elapsed_time, "ms, average_time:", itr->elapsed_time / itr->execute_count, "ms)");
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

}} /* namespace Gamnet */

#endif /* TEST_H_ */
