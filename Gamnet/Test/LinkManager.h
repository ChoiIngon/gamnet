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
		typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
		
	private:
		struct TestExecuteInfo
		{
			TestExecuteInfo()
			{
				name = "";
				execute_count = 0;
				except_count = 0;
				elapsed_time = 0;
				repeat_count = 0;
			}
			std::string name;
			std::atomic_uint execute_count;
			std::atomic_uint except_count;
			uint64_t elapsed_time;
			int repeat_count;
			SEND_HANDLER_TYPE send_handler;
			std::map<uint32_t, RECV_HANDLER_TYPE> recv_handlers;
		};

		std::map<std::string, std::shared_ptr<TestExecuteInfo>>	execute_infos;
		std::map<uint32_t, RECV_HANDLER_TYPE> 	global_recv_handlers;

		Log::Logger	log;
		Timer 		log_timer;
		
		std::atomic_int	begin_execute_count;
		std::atomic_int	finish_execute_count;
		int 			max_execute_count;
		int				session_count;

	public:
		std::string host;
		int			port;

		TestHandler<SESSION_T> test_handler;
		std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;

		LinkManager() : log_timer(GetIOService()), begin_execute_count(0), finish_execute_count(0), max_execute_count(0), host(""), port(0)
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

		void Init(const char* host, int port, int interval, int session_count, int execute_count)
		{
			RegisterTestcase("__connect__", false);
			RegisterTestcase("__close__", true);

			log.Init("test", "test", 5);
			if (0 == interval)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidArgumentError, " 'interval' should be set");
			}
			if (0 == session_count)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidArgumentError, " 'session_count' should be set");
			}

			this->host = host;
			this->port = port;
			this->begin_execute_count = 0;
			this->finish_execute_count = 0;
			this->max_execute_count = execute_count;
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
				std::shared_ptr<Network::Link> link = this->Connect(host.c_str(), port, 5);
				if (nullptr == link)
				{
					throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[link_manager:", this->name, "] can not create 'Test::Link' instance");
				}
			}
		}

		virtual std::shared_ptr<Network::Link> Create() override
		{
			std::shared_ptr<Network::Link> link = this->link_pool.Create();
			if (nullptr == link)
			{
				LOG(ERR, "[", name, "/0/0] can not create link instance");
				return nullptr;
			}

			std::shared_ptr<SESSION_T> session = this->session_pool.Create();
			if (nullptr == session)
			{
				LOG(ERR, "[", name, "/", link->link_key, "/0] can not create session instance");
				return nullptr;
			}
				
			link->session = session;
			session->AttachLink(link);
			session->OnCreate();
			return link;
		}
		
		virtual void OnConnect(const std::shared_ptr<Network::Link>& link) override
		{
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
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
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
			if (nullptr == session)
			{
				return;
			}

			if(true == session->is_connected)
			{
				session->OnClose(reason);
			}
			session->OnDestroy();
			session->AttachLink(nullptr);
			
			finish_execute_count += 1;
			if (max_execute_count > begin_execute_count++)
			{
				std::shared_ptr<Network::Link> link = this->Connect(host.c_str(), port, 5);
				if (nullptr == link)
				{
					LOG(ERR, "[", name, "/0/0] can not create 'Test::Link' instance");
					return;
				}
			}
		}

		virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override
		{
			const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
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
						LOG(WRN, "[", link->link_manager->name, "/", link->link_key, "/", session->session_key, "] can't find handler function(link_key:", link->link_key, ", msg_id:", packet->msg_id, ")");
						link->Close(ErrorCode::InvalidHandlerError);
						return;
					}
				}

				RECV_HANDLER_TYPE& handler = itr->second;
				try {
					session->is_pause = true;
					handler(session, packet);
					if(session->recv_seq < packet->msg_seq)
					{
						session->recv_seq = packet->msg_seq;
					}
				}
				catch (const Exception& e)
				{
					execute_info->except_count++;
					link->Close(e.error_code());
					return;
				}

				if (true == packet->reliable)
				{
					this->test_handler.Send_ReliableAck_Ntf(session);
				}

				if (false == session->is_pause && (int)this->execute_order.size() > session->test_seq)
				{
					const std::shared_ptr<TestExecuteInfo>& next_execute_info = this->execute_order[session->test_seq];
					try
					{
						session->send_time = std::chrono::steady_clock::now();
						next_execute_info->send_handler(session);
					}
					catch (const Gamnet::Exception& e)
					{
						LOG(ERR, e.what(), "(error_code:", e.error_code(), ")");
						link->Close(ErrorCode::UndefinedError);
						return;
					}
					next_execute_info->execute_count++;
				}
			}
			else
			{
				link->Close(ErrorCode::Success);
			}
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

		void OnLogTimerExpire()
		{
			log.Write(GAMNET_INF, "[Test] link count..(active:", this->Size(), ", available:", this->link_pool.Available(), ", max:", this->link_pool.Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] session count..(active:", this->session_manager.Size(), ", available:", this->session_pool.Available(), ", max:", this->session_pool.Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] begin count..(", begin_execute_count, "/", max_execute_count, ")");

			for (auto itr : execute_order)
			{
				if (0 < itr->execute_count)
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", count:", itr->execute_count, ", except:", itr->except_count, ", elapsed:", itr->elapsed_time, "ms, average:", itr->elapsed_time / itr->execute_count, "ms)");
				}
				else
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", count:", itr->execute_count, ", except:0, elapsed:0ms, average:0ms)");
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
