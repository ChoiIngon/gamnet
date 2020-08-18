#ifndef USER_SESSION_H_
#define USER_SESSION_H_

#include <Gamnet/Gamnet.h>
#include <Gamnet/Library/Component.h>
#include <Gamnet/Library/Time/DateTime.h>
#include "../idl/MessageCommon.h"

enum class DatabaseType
{
	Invalid = 0,
	Account = 1,
	User = 100,
};

class UserSession : public Gamnet::Network::Tcp::Session
{
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;

	void StartTransaction();
	void Commit();

	template <class T>
	std::shared_ptr<T> AddComponent()
	{
		return components.AddComponent<T>();
	}

	template <class T>
	std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component)
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

	int shard_index;
	uint64_t user_seq;
	std::shared_ptr<Gamnet::Database::MySQL::Transaction> queries;
	std::shared_ptr<Gamnet::Database::MySQL::Transaction> logs;
	std::map<std::string, std::function<void()>> on_commit;
private :
	Gamnet::Component components;
};

class TestSession : public Gamnet::Test::Session 
{
public:
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;

	template <class T>
	std::shared_ptr<T> AddComponent()
	{
		return components.AddComponent<T>();
	}

	template <class T>
	std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component)
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
	std::map<uint32_t, Message::CounterData> counters;
	std::map<uint64_t, Message::MailData> mails;
	std::map<uint64_t, Message::ItemData> items;
private:
	Gamnet::Component components;
};

#endif /* SESSION_H_ */
