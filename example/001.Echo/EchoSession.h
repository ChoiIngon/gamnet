#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet.h>
#include "EchoMessage.h"

class EchoSession : public Gamnet::Network::Tcp::Session {
public:
	EchoSession();
	virtual ~EchoSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

class TestSession : public Gamnet::Test::Session {
public:
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

#endif