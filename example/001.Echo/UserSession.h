#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet.h>
#include "../idl/Message.h"

class UserSession : public Gamnet::Network::Tcp::Session {
public:
	UserSession();
	virtual ~UserSession();
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