#ifndef _GAMNET_LIB_DELEGATE_H_
#define _GAMNET_LIB_DELEGATE_H_

#include <list>
#include <functional>

namespace Gamnet {
	template <class T> class Delegate;
	template <class R, class... ARGS>
	class Delegate<R(ARGS...)>
	{
		std::list<std::function<R(ARGS...)>> funcs; 
	public :
		typedef typename std::list<std::function<R(ARGS...)>>::iterator iterator;

		R operator () (const ARGS... args)
		{
			R result;
			for(auto func : funcs)
			{
				result = func(args...);
			}
			return result;
		}		
	
		Delegate<R(ARGS...)>& operator += (std::function<R(ARGS...)> const& arg) 
		{
			funcs.push_back(arg);
			return *this;
		}
		
		Delegate<R(ARGS...)>& operator -= (std::function<R(ARGS...)> const& arg)
		{
			R (*const* ptr)(ARGS...) = arg.template target<R(*)(ARGS...)>();
			if(nullptr == ptr)
			{
				return *this;
			}	
	
			for(auto itr=funcs.begin(); itr!=funcs.end(); itr++ )
			{
				R (*const* delegate_ptr)(ARGS...) = (*itr).template target<R(*)(ARGS...)>();
				if(nullptr != delegate_ptr && *ptr == *delegate_ptr) 
				{
					funcs.erase(itr);
					return *this;
				}
			}
	
			return *this;
		}
	
		iterator begin() noexcept
		{
			return funcs.begin();
		}
		iterator end() noexcept
		{
			return funcs.end();
		}
		void clear()
		{
			funcs.clear();
		}
	};

	template <class... ARGS>
	class Delegate<void(ARGS...)>
	{
		std::list<std::function<void(ARGS...)>> funcs; 
	public :
		typedef typename std::list<std::function<void(ARGS...)>>::iterator iterator;

		void operator () (const ARGS... args)
		{
			for(auto func : funcs)
			{
				func(args...);
			}
		}		
	
		Delegate<void(ARGS...)>& operator += (std::function<void(ARGS...)> const& arg) 
		{
			funcs.push_back(arg);
			return *this;
		}
		
		Delegate<void(ARGS...)>& operator -= (std::function<void(ARGS...)> const& arg)
		{
			void (*const* ptr)(ARGS...) = arg.template target<void(*)(ARGS...)>();
			if(nullptr == ptr)
			{
				return *this;
			}	
	
			for(auto itr=funcs.begin(); itr!=funcs.end(); itr++ )
			{
				void (*const* delegate_ptr)(ARGS...) = (*itr).template target<void(*)(ARGS...)>();
				if(nullptr != delegate_ptr && *ptr == *delegate_ptr) 
				{
					funcs.erase(itr);
					return *this;
				}
			}
	
			return *this;
		}
	
		iterator begin() noexcept
		{
			return funcs.begin();
		}
		iterator end() noexcept
		{
			return funcs.end();
		}
		void clear()
		{
			funcs.clear();
		}
	};
}
#endif
