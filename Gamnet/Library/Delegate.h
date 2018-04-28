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
		void operator () (const ARGS... args)
		{
			for(auto func : funcs)
			{
				func(args...);
			}
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
	
		void Clear()
		{
			funcs.clear();
		}
	};
}
#endif
