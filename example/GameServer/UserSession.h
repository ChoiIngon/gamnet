#ifndef USER_SESSION_H_
#define USER_SESSION_H_

#include <Gamnet/Gamnet.h>
#include <Gamnet/Library/Component.h>
#include <Gamnet/Library/Time/DateTime.h>
#include <idl/MessageCommon.h>

class DatabaseType
{
public :
	static constexpr int Invalid = 0;
	static constexpr int Account = 1;
	static constexpr int User = 100;
};

namespace Component
{
	class UserData;
	class Account;
}

class UserSession : public Gamnet::Network::Tcp::Session
{
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;

	int64_t			user_no;
	int32_t			shard_index;

	std::shared_ptr<Component::Account> pAccount;
	std::shared_ptr<Component::UserData> pUserData;

	class Manager
	{
	public :
		void Init();
		std::shared_ptr<UserSession> AddSession(std::shared_ptr<UserSession> session);
		void RemoveSession(std::shared_ptr<UserSession> session);
	private :
		std::mutex lock;
		std::map<int64_t, std::shared_ptr<UserSession>> sessions;
	};
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
	std::map<Message::CounterType, Message::CounterData> counters;
	std::map<int64_t, Message::MailData> mails;

	int cheat_item_order;
	static std::vector<int> item_index;
private:
	Gamnet::Component components;
};

#endif /* SESSION_H_ */
