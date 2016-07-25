/*
 * Test.h
 *
 *  Created on: Jun 9, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_TEST_TEST_H_
#define GAMNET_TEST_TEST_H_

#include <string>
#include "Tester.h"
#include "Session.h"
#include "../Library/Singleton.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace Gamnet { namespace Test {
	template <class SESSION_T>
	void Init(unsigned int interval, unsigned int session_count, unsigned int loop_count, const char* host, int port)
	{
		Singleton<Tester<SESSION_T>>::GetInstance().Init(host, port, interval, session_count, loop_count);
	}

	template<class SESSION_T, class NTF_T>
	bool RegisterHandler(typename Tester<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		Singleton<Tester<SESSION_T>>::GetInstance().template RegisterHandler<NTF_T>(recv);
		return true;
	}

	template<class SESSION_T, class REQ_T, class ANS_T>
	bool RegisterHandler(const std::string& test_name, typename Tester<SESSION_T>::SEND_HANDER_TYPE send, typename Tester<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		Singleton<Tester<SESSION_T>>::GetInstance().template RegisterHandler<REQ_T, ANS_T>(test_name, send, recv);
		return true;
	}

	template<class SESSION_T>
	void Run()
	{
		Singleton<Tester<SESSION_T>>::GetInstance().Run();
	}

	template<class SESSION_T>
	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		const std::string host = ptree_.get<std::string>("server.test.<xmlattr>.host");
		uint32_t port = ptree_.get<uint32_t>("server.test.<xmlattr>.port");
		uint32_t interval = ptree_.get<uint32_t>("server.test.<xmlattr>.interval");
		uint32_t session_count = ptree_.get<uint32_t>("server.test.<xmlattr>.session_count");
		uint32_t loop_count = ptree_.get<uint32_t>("server.test.<xmlattr>.loop_count");
		auto test_case = ptree_.get_child("server.test");
		for(auto elmt : test_case)
		{
			if("message" == elmt.first)
			{
				Singleton<Tester<SESSION_T>>::GetInstance().SetTestSequence(elmt.second.get<std::string>("<xmlattr>.name"));
			}
		}
		Singleton<Tester<SESSION_T>>::GetInstance().Init(host.c_str(), port, interval, session_count, loop_count);
	}
}}

#define GAMNET_BIND_TEST_HANDLER(session_type, send_msg_type, recv_msg_type, send_func, recv_func) \
	static bool Test_##send_msg_type##_##send_func = Gamnet::Test::RegisterHandler<session_type, send_msg_type, recv_msg_type>( \
			#send_msg_type, \
			&send_func, &recv_func \
	)

#define GAMNET_BIND_TEST_ANS_HANDLER(session_type, send_msg_type, recv_msg_type, send_func, recv_func) \
	static bool Test_##send_msg_type##_##send_func = Gamnet::Test::RegisterHandler<session_type, send_msg_type, recv_msg_type>( \
			#send_msg_type, \
			&send_func, &recv_func \
	)

#define GAMNET_BIND_TEST_NTF_HANDLER(session_type, msg_type, func) \
	static bool Test_##msg_type##_##func = Gamnet::Test::RegisterHandler<session_type, msg_type>(func)
#endif /* TEST_H_ */
