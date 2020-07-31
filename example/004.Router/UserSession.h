/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet/Gamnet.h>
#include "../idl/Message.h"

class UserSession : public Gamnet::Network::Tcp::Session 
{
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

class Manager_Session
{
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> cast_group;
public:
	Manager_Session();
	virtual ~Manager_Session();
	void Init();

	void Add(std::shared_ptr<Gamnet::Network::Tcp::Session> session);
	void Remove(std::shared_ptr<Gamnet::Network::Tcp::Session> session);

	template <class MSG>
	bool SendMsg(const MSG& msg, bool reliable = true)
	{
		Gamnet::Network::Tcp::CastGroup::LockGuard lo(cast_group);
		return lo->SendMsg(msg, reliable);
	}
};

class TestSession : public Gamnet::Test::Session {
public :
	std::shared_ptr<Gamnet::Time::Timer> pause_timer;
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

#endif /* SESSION_H_ */
