#ifndef GAMNET_TEST_SESSION_MANAGER_H_
#define GAMNET_TEST_SESSION_MANAGER_H_

#include "../Network/Tcp/SessionManager.h"
#include "../Network/Connector.h"
#include "Session.h"

#include <map>
#include <memory>
#include <vector>
#include <atomic>
#include <functional>

namespace Gamnet {	namespace Test { 
	class SessionManagerImpl
	{
	public :
		Log::Logger						log;
		std::shared_ptr<Time::Timer>	log_timer;
		std::atomic<int64_t>			begin_execute_count;
		std::atomic<int64_t>			finish_execute_count;
		int 							max_execute_count;
		int								session_count;

		std::string				host;
		int						port;

		void Init(const char* host, int port, int session_count, int loop_count);
		
		void Send_Connect_Req(const std::shared_ptr<Session>& session);
		void Recv_Connect_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
		void Send_Reconnect_Req(const std::shared_ptr<Session>& session);
		void Recv_Reconnect_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
		void Send_ReliableAck_Ntf(const std::shared_ptr<Session>& session);
		void Send_Close_Req(const std::shared_ptr<Session>& session);
		void Recv_Close_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);

	private :
		void OnLogTimerExpire();
	};

	template <class SESSION_T>
	class SessionManager : public Network::Tcp::SessionManager<SESSION_T>
	{
		GAMNET_WHERE(SESSION_T, Session);
	private:
		typedef Network::Tcp::SessionManager<SESSION_T> BASE_T;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
		
		struct TestCase
		{
			TestCase() : name(""), execute_count(0), fail_count(0), elapse_time(0) 
			{
			}
			std::string				name;
			std::atomic<int64_t>	execute_count;
			std::atomic<int64_t>	fail_count;
			uint64_t				elapse_time;
			SEND_HANDLER_TYPE		send_handler;
			std::map<uint32_t, RECV_HANDLER_TYPE> receive_handlers;
		};

		std::map<std::string, std::shared_ptr<TestCase>>	test_cases;
		std::map<uint32_t, RECV_HANDLER_TYPE> 				global_recv_handlers;
		std::vector<std::shared_ptr<TestCase>> 				test_sequence;
		
		Network::Connector	connector;
		SessionManagerImpl	impl;
	public:
		SessionManager();
		virtual ~SessionManager();

		void Init(const char* host, int port, int session_count, int loop_count)
		{
			impl.Init(host, port, session_count, loop_count);
			for (size_t i = 0; i < impl.session_count; i++)
			{
				impl.begin_execute_count++;
				if (impl.max_execute_count < impl.begin_execute_count)
				{
					break;
				}
				connector.AsyncConnect(impl.host.c_str(), impl.port, 0);
			}
		}

		virtual std::shared_ptr<Network::Session> OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override;
		virtual void OnReceive(const std::shared_ptr<Network::Session>& session, const std::shared_ptr<Buffer>& buffer) override;
		virtual void OnDestroy(uint32_t sessionKey) override;
		
		void BindSendHandler(const std::string& testName, SEND_HANDLER_TYPE sendHandler);
		void BindRecvHandler(const std::string& testName, uint32_t msgID, RECV_HANDLER_TYPE recv);
		void BindGlobalRecvHandler(uint32_t msgID, RECV_HANDLER_TYPE recv);
		
		void RegisterTestcase(const std::string& test_name)
		{
			auto itr = test_cases.find(test_name);
			if (test_cases.end() == itr)
			{
#ifdef _WIN32
				MessageBoxA(nullptr, Format("can't find registered test case execute info(test_name:", test_name, ")").c_str(), "No Test Execute Info", MB_ICONWARNING);
#endif
				throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", test_name, ")");
			}

			std::shared_ptr<TestCase> testCase = itr->second;
			test_sequence.push_back(testCase);
		}
	private:
		void Recv_Connect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			impl.Recv_Connect_Ans(session, packet);
		}
		
		void Recv_Reconnect_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			impl.Recv_Reconnect_Ans(session, packet);
		}
		void Recv_Close_Ans(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
		{
			impl.Recv_Close_Ans(session, packet);
		}
	};

	template <class SESSION_T>
	SessionManager<SESSION_T>::SessionManager() : connector(this)
	{
		BindGlobalRecvHandler((uint32_t)Network::Tcp::MsgID_SvrCli_Connect_Ans, std::bind(&SessionManager<SESSION_T>::Recv_Connect_Ans, this, std::placeholders::_1, std::placeholders::_2));
		BindGlobalRecvHandler((uint32_t)Network::Tcp::MsgID_SvrCli_Reconnect_Ans, std::bind(&SessionManager<SESSION_T>::Recv_Reconnect_Ans, this, std::placeholders::_1, std::placeholders::_2));
		BindGlobalRecvHandler((uint32_t)Network::Tcp::MsgID_SvrCli_Close_Ans, std::bind(&SessionManager<SESSION_T>::Recv_Close_Ans, this, std::placeholders::_1, std::placeholders::_2));
	}

	template <class SESSION_T>
	SessionManager<SESSION_T>::~SessionManager() {}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindSendHandler(const std::string& testName, typename SessionManager<SESSION_T>::SEND_HANDLER_TYPE sendHandler)
	{
		std::shared_ptr<TestCase> testCase = std::make_shared<TestCase>();
		testCase->name = testName;
		testCase->send_handler = sendHandler;
		if (false == test_cases.insert(std::make_pair(testCase->name, testCase)).second)
		{
#ifdef _WIN32
			MessageBoxA(nullptr, Format("duplicate test case name(test_name:", testCase->name, ")").c_str(), "Duplicate Test Execute Info", MB_ICONWARNING);
#endif
			GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "duplicate test case name(", testCase->name, ")");
		}
	}

	template <class SESSION_T>
	void SessionManager<SESSION_T>::BindRecvHandler(const std::string& messageName, uint32_t msgID, typename SessionManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		auto itr = test_cases.find(handlerName);
		if (test_cases.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "can't find registered test case execute info(test_name:", handlerName, ")");
		}
		std::shared_ptr<TestCase> executeInfo = itr->second;
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
			impl.Send_Connect_Req(session);
		}
		else
		{
			impl.Send_Reconnect_Req(session);
		}
		return session;
	}
	
	template <class SESSION_T>
	void SessionManager<SESSION_T>::OnReceive(const std::shared_ptr<Network::Session>& networkSession, const std::shared_ptr<Buffer>& buffer)
	{
		const std::shared_ptr<SESSION_T> session = std::static_pointer_cast<SESSION_T>(networkSession);
		const std::shared_ptr<Network::Tcp::Packet>& packet = std::static_pointer_cast<Network::Tcp::Packet>(buffer);

		if (session->test_seq < (int)this->test_sequence.size())
		{
			const std::shared_ptr<TestCase> testCase = this->test_sequence[session->test_seq];
			auto itr = testCase->receive_handlers.find(packet->msg_id);
			if (testCase->receive_handlers.end() != itr)
			{
				testCase->elapse_time += Time::Now() - session->send_time;
			}
			else
			{
				itr = global_recv_handlers.find(packet->msg_id);
				if (global_recv_handlers.end() == itr)
				{
					throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't find handler function(msg_id:", packet->msg_id, ")");
				}
			}

			RECV_HANDLER_TYPE& recvHandler = itr->second;
			try {
				recvHandler(session, packet);
			}
			catch (const Exception& e)
			{
				testCase->fail_count++;
			}

			if (session->recv_seq < packet->msg_seq)
			{
				session->recv_seq = packet->msg_seq;
			}

			if (true == packet->reliable)
			{
				impl.Send_ReliableAck_Ntf(session);
			}
		}
		else
		{
			impl.Send_Close_Req(session);
		}
	}
	
	template <class SESSION_T>
	void SessionManager<SESSION_T>::OnDestroy(uint32_t sessionKey) 
	{
		SessionManager<SESSION_T>::BASE_T::OnDestroy(sessionKey);
		impl.finish_execute_count += 1;
		if (impl.max_execute_count > impl.begin_execute_count)
		{
			impl.begin_execute_count++;
			connector.AsyncConnect(impl.host.c_str(), impl.port, 0);
		}
	}
}} /* namespace Gamnet */

#endif /* TEST_H_ */
