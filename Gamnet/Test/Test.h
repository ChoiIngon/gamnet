/*
 * Test.h
 *
 *  Created on: Jun 9, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_TEST_TEST_H_
#define GAMNET_TEST_TEST_H_

#include "Tester.h"
#include "Session.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Test {
	template <class SESSION_T>
	void Init(unsigned int elapsed_time, unsigned int session_count, unsigned int loop_count)
	{
		Singleton<Tester<SESSION_T>>().Init(elapsed_time, session_count, loop_count);
	}

	template<class SESSION_T, class NTF_T>
	void RegisterHandler(typename Tester<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		Singleton<Tester<SESSION_T>>().RegisterHandler<NTF_T>(recv);
	}

	template<class SESSION_T, class REQ_T, class ANS_T>
	void RegisterHandler(typename Tester<SESSION_T>::SEND_HANDER_TYPE send, typename Tester<SESSION_T>::RECV_HANDLER_TYPE recv)
	{
		Singleton<Tester<SESSION_T>>().RegisterHandler<REQ_T, ANS_T>(send, recv);
	}

	template<class SESSION_T>
	void Run(const char* host, int port)
	{
		Singleton<Tester<SESSION_T>>().Run(host, port);
	}
}}

#endif /* TEST_H_ */
