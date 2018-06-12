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
		struct RecvHandlerInfo
		{
			uint32_t msg_id;
			int increase_test_seq; // 1
			RECV_HANDLER_TYPE recv_handler;
		};

		struct TestExecuteInfo
		{
			TestExecuteInfo()
			{
				name = "";
				execute_count = 0;
				elapsed_time = 0;
			}
			std::string name;
			std::atomic_uint execute_count;
			uint64_t elapsed_time;
			SEND_HANDLER_TYPE send_handler;
			std::map<uint32_t, std::shared_ptr<RecvHandlerInfo>> recv_handlers;
		};

		std::map<std::string, std::shared_ptr<TestExecuteInfo>>	execute_infos;
		std::map<std::string, std::list<std::shared_ptr<RecvHandlerInfo>>> 	recv_handlers;

		Log::Logger	log;
		Timer 		log_timer;
		Timer 		execute_timer;

		std::atomic_int	begin_execute_count;
		std::atomic_int	finish_execute_count;
		int 			max_execute_count;
		int				session_count;

		std::string host;
		int			port;
		
	public:
		TestHandler<SESSION_T> test_handler;
		std::vector<std::shared_ptr<TestExecuteInfo>> 			execute_order;

		LinkManager() : log_timer(GetIOService()), begin_execute_count(0), finish_execute_count(0), max_execute_count(0), host(""), port(0)
		{
			this->name = "Gamnet::Test::LinkManager";

			RegisterSendHandler("__connect__", 
				std::bind(&TestHandler<SESSION_T>::Send_Connect_Req, &test_handler, std::placeholders::_1)
			);
			RegisterRecvHandler("__connect__", 
				(uint32_t)Network::Tcp::MsgID_SvrCli_Connect_Ans, 
				std::bind(&TestHandler<SESSION_T>::Recv_Connect_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2), 
				1
			);
			RegisterRecvHandler("__connect__", 
				(uint32_t)Network::Tcp::MsgID_SvrCli_Reconnect_Ans, 
				std::bind(&TestHandler<SESSION_T>::Recv_Reconnect_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2), 
				1
			);
			RegisterSendHandler("__close__", 
				std::bind(&TestHandler<SESSION_T>::Send_Close_Req, &test_handler, std::placeholders::_1)
			);
			RegisterRecvHandler("__close__", 
				(uint32_t)Network::Tcp::MsgID_SvrCli_Close_Ans, std::bind(&TestHandler<SESSION_T>::Recv_Close_Ans, &test_handler, std::placeholders::_1, std::placeholders::_2), 
				1
			);
		}

		virtual ~LinkManager()
		{
		}

		void Init(const char* host, int port, int interval, int session_count, int execute_count)
		{
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

			execute_timer.AutoReset(true);
			execute_timer.SetTimer(interval, std::bind(&LinkManager<SESSION_T>::OnExecuteTimerExpire, this));

			log_timer.AutoReset(true);
			log_timer.SetTimer(3000, std::bind(&LinkManager<SESSION_T>::OnLogTimerExpire, this));

			Test::CreateThreadPool(std::thread::hardware_concurrency());
		}
			
		virtual std::shared_ptr<Network::Link> Create() override
		{
			std::shared_ptr<Network::Link> link = this->link_pool.Create();
			if (nullptr == link)
			{
				LOG(GAMNET_ERR, "[link_manager:", this->name, "] can not create 'Test::Link' instance");
				return nullptr;
			}

			std::shared_ptr<SESSION_T> session = this->session_pool.Create();
			if (nullptr == session)
			{
				LOG(ERR, "[link_manager:", this->name, "] can not create session. connect fail(session count:", this->session_manager.Size(), "/", this->session_pool.Capacity(), ")");
				return nullptr;
			}
				
			link->session = session;
			session->strand.wrap([session, link]() {
				try {
					session->OnCreate();
					session->AttachLink(link);
				}
				catch (const Exception& e)
				{
					LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
				}
			})();
			return link;
		}

		virtual void OnConnect(const std::shared_ptr<Network::Link>& link) override
		{
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
			if (nullptr == session)
			{
				link->Close(ErrorCode::InvalidSessionError);
				LOG(ERR, "[link_manager:", this->name, "] can not create session. connect fail(session count:", this->session_manager.Size(), "/", this->session_pool.Capacity(), ")");
				return;
			}
				
			this->session_manager.Add(session->session_key, session);

			session->strand.wrap([this, session]() {
				try {
					if (0 < this->execute_order.size())
					{
						if (0 == session->test_seq)
						{
							const std::shared_ptr<TestExecuteInfo>& info = this->execute_order[0];
							session->send_time = std::chrono::steady_clock::now();
							info->send_handler(session);
							info->execute_count++;
						}
						else
						{
							this->test_handler.Send_Reconnect_Req(session);
						}
					}
				}
				catch (const Exception& e)
				{
					LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
				}
			})();
		}

		virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override
		{
			std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
			if (nullptr == session)
			{
				return;
			}

			session->strand.wrap([session, reason]() {
				try {
					if(true == session->is_connected)
					{
						session->OnClose(reason);
					}
					session->AttachLink(nullptr);
					session->OnDestroy();
				}
				catch (const Exception& e)
				{
					LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
				}
			})();

			this->session_manager.Remove(session->session_key);
			finish_execute_count++;
		}

		virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override
		{
			const std::shared_ptr<Network::Tcp::Packet>& packet = std::static_pointer_cast<Network::Tcp::Packet>(buffer);
			const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(link->session);
			if (nullptr == session)
			{
				LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
				link->Close(ErrorCode::InvalidSessionError);
				return;
			}

			session->strand.wrap([this, link, session, packet]() {
				if (session->test_seq < (int)this->execute_order.size())
				{
					bool is_global_handler = false;

					const std::shared_ptr<TestExecuteInfo>& execute_info = this->execute_order[session->test_seq];
					auto itr = execute_info->recv_handlers.find(packet->msg_id);
					if (execute_info->recv_handlers.end() == itr)
					{
						itr = this->execute_order[0]->recv_handlers.find(packet->msg_id);
						if (this->execute_order[0]->recv_handlers.end() == itr)
						{
							LOG(GAMNET_WRN, "can't find handler function(msg_id:", packet->msg_id, ")");
							link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::InvalidHandlerError))();
							return;
						}

						is_global_handler = true;
					}

					RECV_HANDLER_TYPE& handler = itr->second->recv_handler;
					try {
						handler(session, packet);
						if(session->recv_seq < packet->msg_seq)
						{
							session->recv_seq = packet->msg_seq;
						}
					}
					catch (const Exception& e)
					{
						link->strand.wrap(std::bind(&Network::Link::Close, link, e.error_code()))();
						return;
					}

					if (true == packet->reliable)
					{
						this->test_handler.Send_ReliableAck_Ntf(session);
					}

					if (false == is_global_handler)
					{
						auto now = std::chrono::steady_clock::now();
						execute_info->elapsed_time += std::chrono::duration_cast<std::chrono::milliseconds>(now - session->send_time).count();
						session->send_time = now;
					}

					if (false == session->is_pause && 0 < itr->second->increase_test_seq)
					{
						session->test_seq += itr->second->increase_test_seq;
						if (session->test_seq >= (int)this->execute_order.size())
						{
							this->test_handler.Send_Close_Req(session);
							return;
						}

						const std::shared_ptr<TestExecuteInfo>& next_execute_info = this->execute_order[session->test_seq];
						try
						{
							session->send_time = std::chrono::steady_clock::now();
							next_execute_info->send_handler(session);
						}
						catch (const Gamnet::Exception& e)
						{
							LOG(ERR, e.what(), "(error_code:", e.error_code(), ")");
							link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::UndefinedError))();
							return;
						}
						next_execute_info->execute_count++;
					}
				}
			})();
		}

		void RegisterSendHandler(const std::string& test_name, SEND_HANDLER_TYPE send)
		{
			std::shared_ptr<TestExecuteInfo> executeInfo = std::shared_ptr<TestExecuteInfo>(new TestExecuteInfo());
			executeInfo->name = test_name;
			executeInfo->send_handler = send;
			if (false == execute_infos.insert(std::make_pair(test_name, executeInfo)).second)
			{
				GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "(duplicate test case name(", test_name, ")");
			}
		}

		void RegisterRecvHandler(const std::string& test_name, uint32_t msgID, RECV_HANDLER_TYPE recv, int increaseTestSEQ = 1)
		{
			std::shared_ptr<RecvHandlerInfo> recvHandlerInfo = std::shared_ptr<RecvHandlerInfo>(new RecvHandlerInfo());
			recvHandlerInfo->msg_id = msgID;
			recvHandlerInfo->increase_test_seq = increaseTestSEQ;
			recvHandlerInfo->recv_handler = recv;
			std::list<std::shared_ptr<RecvHandlerInfo>>& lstRecvHandlerInfos = recv_handlers[test_name];
			lstRecvHandlerInfos.push_back(recvHandlerInfo);
		}

		void SetTestSequence(const std::string& test_name)
		{
			auto itr_execute_info = execute_infos.find(test_name);
			if (execute_infos.end() == itr_execute_info)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", test_name, ")");
			}
			const std::shared_ptr<TestExecuteInfo>& info = itr_execute_info->second;

			auto itr_recv_handlers = recv_handlers.find(test_name);
			if (recv_handlers.end() == itr_recv_handlers)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case recv handler(test_name:", test_name, ")");
			}

			for (const std::shared_ptr<RecvHandlerInfo> recv_handler : itr_recv_handlers->second)
			{
				info->recv_handlers[recv_handler->msg_id] = recv_handler;
			}
			execute_order.push_back(info);
		}

		void OnExecuteTimerExpire()
		{
			for (size_t i = 0; i<this->session_count && this->begin_execute_count < this->max_execute_count; i++)
			{
				if (this->Size() >= this->session_count || this->link_pool.Available() == 0)
				{
					break;
				}

				std::shared_ptr<Network::Link> link = this->Connect(this->host.c_str(), this->port, 5);
				if (nullptr == link)
				{
					LOG(ERR, "[link_manager:", this->name, "] can not create link. connect fail(link count:", this->Size(), "/", this->link_pool.Capacity(), ")");
					break;
				}

				this->begin_execute_count++;
			}

			if (this->begin_execute_count >= this->max_execute_count)
			{
				this->execute_timer.Cancel();
			}
		}
		void OnLogTimerExpire()
		{
			log.Write(GAMNET_INF, "[Test] execute count..(", begin_execute_count, "/", max_execute_count, ")");
			log.Write(GAMNET_INF, "[Test] link count..(active:", this->Size(), ", available:", this->link_pool.Available(), ", max:", this->link_pool.Capacity(), ")");
			log.Write(GAMNET_INF, "[Test] session count..(active:", this->session_manager.Size(), ", available:", this->session_pool.Available(), ", max:", this->session_pool.Capacity(), ")");

			for (auto itr : execute_order)
			{
				if (0 < itr->execute_count)
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", count:", itr->execute_count, ", elapsed:", itr->elapsed_time, "ms, average:", itr->elapsed_time / itr->execute_count, "ms)");
				}
				else
				{
					log.Write(GAMNET_INF, "[Test] running state..(name:", itr->name, ", count:", itr->execute_count, ", elapsed:0ms, average:0ms)");
				}
			}

			if (finish_execute_count >= max_execute_count)
			{
				log_timer.Cancel();
				log.Write(GAMNET_INF, "[Test] test finished..(", finish_execute_count, "/", max_execute_count, ")");
			}
		}
	};

}} /* namespace Gamnet */

#endif /* TEST_H_ */
