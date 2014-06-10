/*
 * HandlerEcho.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#include "HandlerEcho.h"
#include "Message.h"

HandlerEcho::HandlerEcho() {
}

HandlerEcho::~HandlerEcho() {
}

void HandlerEcho::Recv(std::shared_ptr<ClientSession> client, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	Msg_CS_Echo_Req req;
	Msg_SC_Echo_Ans	ans;
	Msg_SS_Echo_Ntf ntf;
	try {
		if(false == Gamnet::Network::Packet::Load(req, packet))
		{
			throw Gamnet::Exception("packet load fail");
		}
		ans.Message = req.Message;
		ntf.Message = req.Message;
	}
	catch(const Gamnet::Exception& e)
	{
		Gamnet::Log::Write(GAMNET_ERR, e.what());
	}
	Gamnet::Router::Address addr;
	addr.service_name = "HELLOWORLD";
	addr.cast_type = ROUTER_CAST_MULTI;
	Gamnet::Router::SendMsg(client, addr, ntf);
	Gamnet::Network::SendMsg(client, ans);
}

static bool HandlerEcho_Recv_Result = Gamnet::Network::RegisterHandler<ClientSession>(
	Msg_CS_Echo_Req::MSG_ID,
	&HandlerEcho::Recv,
	std::shared_ptr<Gamnet::Network::HandlerStatic<HandlerEcho>>(new Gamnet::Network::HandlerStatic<HandlerEcho>())
);

void Test_Echo_Req(std::shared_ptr<TestSession> client)
{
	Msg_CS_Echo_Req req;
	req.Message = "Hello World!";
	Gamnet::Network::SendMsg(client, req);
}

void Test_Echo_Ans(std::shared_ptr<TestSession> client, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	Msg_SC_Echo_Ans ans;
	try {
		if(false == Gamnet::Network::Packet::Load(ans, packet))
		{
			throw Gamnet::Exception("packet load fail");
		}
	}
	catch(const Gamnet::Exception& e)
	{
		Gamnet::Log::Write(GAMNET_ERR, e.what());
		return;
	}
	Gamnet::Log::Write(GAMNET_INF, ans.Message);
}
