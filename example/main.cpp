/*
 * main.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */
#include <Gamnet.h>
#include "ClientSession.h"
#include "Message.h"
#include "HandlerEcho.h"
#include <Router/Router.h>

int main()
{
	Gamnet::Log::ReadXml("server.xml");
	Gamnet::Network::Listen<ClientSession>(20000, 1024, 60);
	Gamnet::Router::Listen("HELLOWORLD");
	Gamnet::Test::Init<TestSession>(100, 50, 100, "localhost", 20000);
	Gamnet::Test::Run<TestSession>();
	Gamnet::Run(20);
	return 0;
}



