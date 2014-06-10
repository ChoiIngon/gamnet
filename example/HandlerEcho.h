/*
 * HandlerEcho.h
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#ifndef HANDLERECHO_H_
#define HANDLERECHO_H_
#include <Gamnet.h>
#include "ClientSession.h"
#include "TestSession.h"

class HandlerEcho : public Gamnet::Network::IHandler {
public:
	HandlerEcho();
	virtual ~HandlerEcho();

	void Recv(std::shared_ptr<ClientSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
};

void Test_Echo_Req(std::shared_ptr<TestSession> client);
void Test_Echo_Ans(std::shared_ptr<TestSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
#endif /* HANDLERECHO_H_ */
