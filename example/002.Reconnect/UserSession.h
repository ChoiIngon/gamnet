#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet.h>
#include <Library/Component.h>
#include "../idl/Message.h"

class UserSession 
	: public Gamnet::Network::Tcp::Session
{
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
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
};

#endif