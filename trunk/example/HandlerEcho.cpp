/*
 * HandlerEcho.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#include "HandlerEcho.h"
#include "Message.h"
#include <Router/Router.h>

HandlerEcho::HandlerEcho() {
}

HandlerEcho::~HandlerEcho() {
}

void HandlerEcho::Recv_CS_Req(std::shared_ptr<ClientSession> client, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	Msg_CS_Echo_Req cs_req;
	Msg_SC_Echo_Ans	sc_ans;
	Msg_SS_Echo_Req ss_req;

	try {
		if(false == Gamnet::Network::Packet::Load(cs_req, packet))
		{
			throw Gamnet::Exception("packet load fail");
		}
		sc_ans.Message = cs_req.Message;
		ss_req.Message = cs_req.Message;

		Gamnet::Router::Address addr;
		addr.service_name = "HELLOWORLD";
		addr.cast_type = Gamnet::Router::ROUTER_CAST_MULTI;
		BindMessage<Msg_SS_Echo_Ans>(client);
		Gamnet::Router::SendMsg(client, addr, ss_req);
	}
	catch(const Gamnet::Exception& e)
	{
		Gamnet::Log::Write(GAMNET_ERR, e.what());
	}

	Gamnet::Network::SendMsg(client, sc_ans);
}

static bool HandlerEcho_Recv_CS_Req_Ret = Gamnet::Network::RegisterHandler<ClientSession>(
	Msg_CS_Echo_Req::MSG_ID,
	&HandlerEcho::Recv_CS_Req,
	std::shared_ptr<Gamnet::Network::HandlerCreate<HandlerEcho>>(new Gamnet::Network::HandlerCreate<HandlerEcho>())
);

void HandlerEcho::Recv_SS_Ans(const Gamnet::Router::Address& from, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	Msg_SS_Echo_Ans ans;
	try {
		if(false == Gamnet::Network::Packet::Load(ans, packet))
		{
			throw Gamnet::Exception("packet load fail");
		}

	}
	catch(const Gamnet::Exception& e)
	{
		Gamnet::Log::Write(GAMNET_ERR, e.what());
	}
	Gamnet::Log::Write(GAMNET_DEV, ans.Message);
}
static bool HandlerEcho_Recv_SS_Ans_Ret = Gamnet::Router::RegisterHandler(
	Msg_SS_Echo_Ans::MSG_ID,
	&HandlerEcho::Recv_SS_Ans,
	std::shared_ptr<Gamnet::Network::HandlerFind<HandlerEcho>>(new Gamnet::Network::HandlerFind<HandlerEcho>())
);

void HandlerEcho::Recv_SS_Req(const Gamnet::Router::Address& from, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	Msg_SS_Echo_Req req;
	Msg_SS_Echo_Ans ans;
	try {
		if(false == Gamnet::Network::Packet::Load(req, packet))
		{
			throw Gamnet::Exception("packet load fail");
		}
		ans.Message = req.Message;
	}
	catch(const Gamnet::Exception& e)
	{
		Gamnet::Log::Write(GAMNET_ERR, e.what());
	}
	Gamnet::Log::Write(GAMNET_DEV, req.Message);
	Gamnet::Router::SendMsg(from, ans);
}

static bool Recv_SS_Ntf_Result = Gamnet::Router::RegisterHandler(
	Msg_SS_Echo_Req::MSG_ID,
	&HandlerEcho::Recv_SS_Req,
	std::shared_ptr<Gamnet::Network::HandlerCreate<HandlerEcho>>(new Gamnet::Network::HandlerCreate<HandlerEcho>())
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
	Gamnet::Log::Write(GAMNET_DEV, ans.Message);
}
