/*
 * HandlerEcho.h
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#ifndef HANDLERECHO_H_
#define HANDLERECHO_H_
#include <Gamnet.h>
#include <Router/Router.h>
#include "ClientSession.h"
#include "TestSession.h"

class HandlerEcho : public Gamnet::Network::IHandler {
public:
	HandlerEcho();
	virtual ~HandlerEcho();

	void Recv_CS_Req(std::shared_ptr<ClientSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
	void Recv_SS_Ntf(const Gamnet::Router::Address& from, std::shared_ptr<Gamnet::Network::Packet> packet);
	void Recv_SC_Ans(std::shared_ptr<ClientSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
	void Recv_SS_Req(const Gamnet::Router::Address& from, std::shared_ptr<Gamnet::Network::Packet> packet);
};

void Test_Echo_Req(std::shared_ptr<TestSession> client);
void Test_Echo_Ans(std::shared_ptr<TestSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
#endif /* HANDLERECHO_H_ */
