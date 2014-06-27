/*
 * main.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */
#include "ClientSession.h"

int main()
{
	Gamnet::Log::ReadXml("config.xml");
	Gamnet::Network::Listen<ClientSession>(20000, 1024, 60);
	Gamnet::Router::Listen("HELLOWORLD", 20005);
	Gamnet::Router::Connect("localhost", 20005);
	Gamnet::Test::ReadXml<TestSession>("config.xml");
	Gamnet::Test::Run<TestSession>();
	Gamnet::Run(20);
	return 0;
}



