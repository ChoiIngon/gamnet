#ifndef _COMPONENT_USER_DATA_H_
#define _COMPONENT_USER_DATA_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/DateTime.h>
#include "UserData/Bag.h"
#include "UserData/Suit.h"
#include "UserData/Counter.h"

namespace Component 
{
	class UserData
	{
	public:
		UserData();

		int64_t						user_no;	
		std::string					user_name;
		Gamnet::Time::DateTime		offline_time;

		std::shared_ptr<Bag>		pBag;
		std::shared_ptr<Suit>		pSuit;
		std::shared_ptr<Counter>	pCounter;

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

		void Serialize(Message::UserData& userData);
	private:
		Gamnet::Component components;
	};
}
#endif
