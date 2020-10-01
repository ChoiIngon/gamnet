#ifndef __GAMNET_LIB_COMPONENT_H_
#define __GAMNET_LIB_COMPONENT_H_

#include <memory>
#include <map>
#include <string>

namespace Gamnet {
	class Component {
		std::map<std::string, std::shared_ptr<void>> components;
	public :
		template <class T>
		std::shared_ptr<T> AddComponent()
		{
			std::shared_ptr<T> component = std::make_shared<T>();
			components[typeid(T).name()] = component;
			return component;
		}

		template <class T>
		std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component)
		{
			components[typeid(T).name()] = component;
			return component;
		}

		template <class T>
		std::shared_ptr<T> AddComponent(const std::string& name)
		{
			std::shared_ptr<T> component = std::make_shared<T>();
			components[name] = component;
			return component;
		}
		template <class T>
		std::shared_ptr<T> GetComponent()
		{
			auto itr = components.find(typeid(T).name());
			if(components.end() == itr)
			{
				return nullptr;
			}
			return std::reinterpret_pointer_cast<T>(itr->second);
		}

		template <class T>
		void RemoveComponent()
		{
			components.erase(typeid(T).name());
		}

		void Clear()
		{
			components.clear();
		}
	};
}

#endif