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
		std::shared_ptr<T> Add()
		{
			std::shared_ptr<T> component = std::make_shared<T>();
			components[typeid(T).name()] = component;
			return component;
		}

		template <class T>
		std::shared_ptr<T> Add(const std::shared_ptr<T>& component)
		{
			components[typeid(T).name()] = component;
			return component;
		}

		template <class T>
		std::shared_ptr<T> Add(const std::string& name)
		{
			std::shared_ptr<T> component = std::make_shared<T>();
			components[name] = component;
			return component;
		}
		template <class T>
		std::shared_ptr<T> Get()
		{
			auto itr = components.find(typeid(T).name());
			if(components.end() == itr)
			{
				return nullptr;
			}
			return std::static_pointer_cast<T>(itr->second);
		}

        template <class T>
        std::shared_ptr<T> Get(const std::string& name)
        {
            auto itr = components.find(name);
            if (components.end() == itr)
            {
                return nullptr;
            }
            return std::static_pointer_cast<T>(itr->second);
        }

		template <class T>
		void Remove()
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