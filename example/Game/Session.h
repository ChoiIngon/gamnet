#pragma once

#ifndef USER_SESSION_H_
#define USER_SESSION_H_

#include <Gamnet/Gamnet.h>
#include <Gamnet/Library/Component.h>
#include <Gamnet/Library/Time/DateTime.h>

class Session : public Gamnet::Network::Tcp::Session
{
public:
	Session();
	virtual ~Session();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;

	template <class T>
	std::shared_ptr<T> AddComponent()
	{
		return components.Add<T>();
	}
	template <class T>
	std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component)
	{
		return components.Add<T>(component);
	}
	template <class T>
	std::shared_ptr<T> AddComponent(const std::string& name)
	{
		return components.Add<T>(name);
	}
	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		return components.Get<T>();
	}
	template <class T>
	void RemoveComponent()
	{
		components.Remove<T>();
	}

	int shard_index;
	uint64_t user_no;
	
private:
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
		return components.Add<T>();
	}

	template <class T>
	std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component)
	{
		return components.Add<T>(component);
	}

	template <class T>
	std::shared_ptr<T> AddComponent(const std::string& name)
	{
		return components.Add<T>(name);
	}
	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		return components.Get<T>();
	}

	template <class T>
	void RemoveComponent()
	{
		components.Remove<T>();
	}
	
private:
	Gamnet::Component components;
};

#endif /* SESSION_H_ */

