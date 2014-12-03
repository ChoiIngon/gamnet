/*
 * HandlerEcho.h
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#ifndef HANDLERECHO_H_
#define HANDLERECHO_H_
#include "ClientSession.h"
class HandlerEcho : public Gamnet::Network::IHandler {
public:
	HandlerEcho();
	virtual ~HandlerEcho();

	void Recv_CS_Req(std::shared_ptr<ClientSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
	void Recv_SS_Req(const Gamnet::Router::Address& from, std::shared_ptr<Gamnet::Network::Packet> packet);
	void Recv_SS_Ans(const Gamnet::Router::Address& from, std::shared_ptr<Gamnet::Network::Packet> packet);
	void Recv_Http_Req(std::shared_ptr<Http::Session> client, const std::map<std::string, std::string>& param);
};

void Test_Echo_Req(std::shared_ptr<TestSession> client);
void Test_Echo_Ans(std::shared_ptr<TestSession> client, std::shared_ptr<Gamnet::Network::Packet> packet);
#endif /* HANDLERECHO_H_ */
