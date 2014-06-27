/*
 * ClientSession.h
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#ifndef CLIENTSESSION_H_
#define CLIENTSESSION_H_

#include <Gamnet.h>
using namespace Gamnet;

class ClientSession : public Gamnet::Network::Session
{
public :
	virtual void OnAccept() {}
	virtual void OnClose(int reason) {}
};

class TestSession : public Gamnet::Test::Session {
public:
	TestSession() {}
	virtual ~TestSession() {}
};

#endif /* CLIENTSESSION_H_ */
