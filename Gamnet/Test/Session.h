/*
 * TestSession.h
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_
#include "../Network/Network.h"

namespace Gamnet { namespace Test {

class Session : public Network::Tcp::Session {
public:
	int test_seq;

	Session();
	virtual ~Session();

	void OnAccept() {}
	void OnClose(int reason) {}
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
