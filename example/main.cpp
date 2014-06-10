/*
 * main.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */
#include <Gamnet.h>
#include "ClientSession.h"
#include "TestSession.h"
#include "Message.h"
#include "HandlerEcho.h"

int main()
{
	Gamnet::Log::Init("log");
	Gamnet::Network::Listen<ClientSession>(20000, 1024, 60);
	Gamnet::Test::Init<TestSession>(1000, 1, 1);
	Gamnet::Test::RegisterHandler<TestSession, Msg_CS_Echo_Req, Msg_SC_Echo_Ans>(Test_Echo_Req, Test_Echo_Ans);
	Gamnet::Test::Run<TestSession>("localhost", 20000);
	Gamnet::Run();
	return 0;
}


