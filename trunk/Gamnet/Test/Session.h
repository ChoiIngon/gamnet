/*
 * TestSession.h
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#ifndef TESTSESSION_H_
#define TESTSESSION_H_
#include "../Network/Session.h"

namespace Gamnet { namespace Test {

class Session : public Network::Session {
public:
	unsigned int testSEQ_;

	Session();
	virtual ~Session();

	void Connect(const char* host, int port);
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
