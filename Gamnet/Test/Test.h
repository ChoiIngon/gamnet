#ifndef GAMNET_TEST_TEST_H_
#define GAMNET_TEST_TEST_H_

#include <list>
#include <string>

#include "SessionManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Test {
	template <class SESSION_T>
	void Init(const char* host, int port, unsigned int session_count, unsigned int loop_count)
	{
		Singleton<SessionManager<SESSION_T>>::GetInstance().Init(host, port, session_count, loop_count);
	}
	
	template<class SESSION_T, class REQ_T, class ANS_T>
	bool BindHandler(const std::string& test_name, typename SessionManager<SESSION_T>::SEND_HANDLER_TYPE send, typename SessionManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		Singleton<SessionManager<SESSION_T>>::GetInstance().BindSendHandler(test_name, send);
		Singleton<SessionManager<SESSION_T>>::GetInstance().BindRecvHandler(test_name, ANS_T::MSG_ID, recv);
		return true;
	}

	template<class SESSION_T, class MSG_T>
	bool BindRecvHandler(const std::string& test_name, typename SessionManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		if("" != test_name)
		{
			Singleton<SessionManager<SESSION_T>>::GetInstance().BindRecvHandler(test_name, MSG_T::MSG_ID, recv);
		}
		else
		{
			Singleton<SessionManager<SESSION_T>>::GetInstance().BindGlobalRecvHandler(MSG_T::MSG_ID, recv);
		}
		return true;
	}

	template <class MSG>
	void SendMsg(const std::shared_ptr<Session>& session, const MSG& msg, bool reliable = true)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			LOG(ERR, "can not create Packet instance(msg_id:", MSG::MSG_ID, ")");
			return;
		}

		packet->msg_seq = ++session->send_seq;
		packet->reliable = reliable;
		if (false == packet->Write(msg))
		{
			LOG(ERR, "fail to serialize packet(msg_id:", MSG::MSG_ID, ")");
			return;
		}
		
		session->AsyncSend(packet);
	}

	struct Config
	{
		Config();

		std::string host;
		int port;
		int session_count;
		int loop_count;
		std::list<std::string> messages;

		void ReadXml(const std::string& config);
	};
	template<class SESSION_T>
	void ReadXml(const std::string& path)
	{
		Config config;
		config.ReadXml(path);
		if(0 == config.port * config.session_count * config.loop_count)
		{
			return;
		}

		for(const auto& message : config.messages)
		{
			Singleton<SessionManager<SESSION_T>>::GetInstance().RegisterTestcase(message);
		}
		Init<SESSION_T>(config.host.c_str(), config.port, config.session_count, config.loop_count);
	}
}}

#define GAMNET_BIND_TEST_HANDLER(session_type, test_name, send_msg_type, send_func, recv_msg_type, recv_func) \
	static bool TOKEN_PASTE2(Test_##send_func##_##recv_func##_, __LINE__) = Gamnet::Test::BindHandler<session_type, send_msg_type, recv_msg_type>( \
			test_name, \
			&send_func, &recv_func \
	)

#define GAMNET_BIND_TEST_RECV_HANDLER(session_type, test_name, msg_type, recv_func) \
	static bool TOKEN_PASTE2(Test_##recv_func##_, __LINE__) = Gamnet::Test::BindRecvHandler<session_type, msg_type>(test_name, &recv_func)

#endif /* TEST_H_ */
