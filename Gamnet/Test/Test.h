#ifndef GAMNET_TEST_TEST_H_
#define GAMNET_TEST_TEST_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Session.h"
#include "LinkManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Test {
	template <class SESSION_T>
	void Init(const char* host, int port, unsigned int session_count, unsigned int loop_count)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().Init(host, port, session_count, loop_count);
	}
	
	void Run(int thread_count);
	void RegisterRun(std::function<void()> runFunctor);
	
	template<class SESSION_T, class REQ_T, class ANS_T>
	bool BindHandler(const std::string& test_name, typename LinkManager<SESSION_T>::SEND_HANDLER_TYPE send, typename LinkManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().BindSendHandler(test_name, send);
		Singleton<LinkManager<SESSION_T>>::GetInstance().BindRecvHandler(test_name, ANS_T::MSG_ID, recv);
		return true;
	}

	template<class SESSION_T, class MSG_T>
	bool BindRecvHandler(const std::string& test_name, typename LinkManager<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		if("" != test_name)
		{
			Singleton<LinkManager<SESSION_T>>::GetInstance().BindRecvHandler(test_name, MSG_T::MSG_ID, recv);
		}
		else
		{
			Singleton<LinkManager<SESSION_T>>::GetInstance().BindGlobalRecvHandler(MSG_T::MSG_ID, recv);
		}
		return true;
	}

	template <class SESSION_T, class MSG>
	void SendMsg(const std::shared_ptr<SESSION_T>& session, const MSG& msg, bool reliable = false)
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

	template<class SESSION_T>
	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		const std::string host = ptree_.get<std::string>("server.test.<xmlattr>.host");
		uint32_t port = ptree_.get<uint32_t>("server.test.<xmlattr>.port");
		uint32_t session_count = ptree_.get<uint32_t>("server.test.<xmlattr>.session_count");
		uint32_t loop_count = ptree_.get<uint32_t>("server.test.<xmlattr>.loop_count");
		auto test_case = ptree_.get_child("server.test");

		for(const auto& elmt : test_case)
		{
			if("message" == elmt.first)
			{
				Singleton<LinkManager<SESSION_T>>::GetInstance().RegisterTestcase(elmt.second.get<std::string>("<xmlattr>.name"));
			}
		}
		
		Init<SESSION_T>(host.c_str(), port, session_count, loop_count);
		RegisterRun(std::function<void()>(std::bind(&LinkManager<SESSION_T>::Run, &Singleton<LinkManager<SESSION_T>>::GetInstance())));
	}
}}

#define GAMNET_BIND_TEST_HANDLER(session_type, test_name, send_msg_type, send_func, recv_msg_type, recv_func) \
	static bool TOKEN_PASTE2(Test_##send_msg_type##_##send_func##_, __LINE__) = Gamnet::Test::BindHandler<session_type, send_msg_type, recv_msg_type>( \
			test_name, \
			&send_func, &recv_func \
	)

#define GAMNET_BIND_TEST_RECV_HANDLER(session_type, test_name, msg_type, recv_func) \
	static bool TOKEN_PASTE2(Test_##msg_type##_##func##_, __LINE__) = Gamnet::Test::BindRecvHandler<session_type, msg_type>(test_name, &recv_func)

#endif /* TEST_H_ */
