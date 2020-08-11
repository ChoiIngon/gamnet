#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet/Gamnet.h>
#include <Gamnet/Library/Component.h>
#include "../idl/Message.h"

enum class Database
{
	Invalid = 0,
	Gamnet = 1
};

class Counter;
class UserSession : public Gamnet::Network::Tcp::Session
{
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;

	std::shared_ptr<Gamnet::Database::MySQL::Transaction> transaction;
	std::map<uint32_t, std::shared_ptr<Counter>> counters;

	std::shared_ptr<Counter> GetCounter(uint32_t counterID);
	std::shared_ptr<Counter> AddCounter(const std::shared_ptr<Counter>& counter);
	template <class T>
	std::shared_ptr<T> AddComponent()
	{
		return components.AddComponent<T>();
	}

	template <class T>
	std::shared_ptr<T> AddComponent(std::shared_ptr<T>& component)
	{
		return components.AddComponent<T>(component);
	}

	template <class T>
	std::shared_ptr<T> AddComponent(const std::string& name)
	{
		return components.AddComponent<T>(name);
	}
	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		return components.GetComponent<T>();
	}

	template <class T>
	void RemoveComponent()
	{
		components.RemoveComponent<T>();
	}
private :
	Gamnet::Component components;
};

class Counter
{
private :
	std::shared_ptr<UserSession> session;
public :
	uint64_t counter_seq;
	uint32_t counter_id;
	int count;
public :
	Counter(const std::shared_ptr<UserSession>& session, uint64_t counter_seq, uint32_t counter_id, int count);

	int Increase(int amount);
};

class TestSession : public Gamnet::Test::Session 
{
public:
	std::shared_ptr<Gamnet::Time::Timer> pause_timer;
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

#endif /* SESSION_H_ */
