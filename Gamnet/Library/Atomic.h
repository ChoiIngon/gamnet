#ifndef __GAMNET_LIB_ATOMIC_H_
#define __GAMNET_LIB_ATOMIC_H_

#include <mutex>
#include <memory>

namespace Gamnet {
	template <class T>
	class AtomicPtr
	{
		T* ptr;
	public :
		AtomicPtr(const T& obj) : ptr(const_cast<T*>(&obj)) 
		{
			ptr->lock();
		}
		AtomicPtr(const T* obj) : ptr(const_cast<T*>(obj)) 
		{
			ptr->lock();
		}
		AtomicPtr(const std::shared_ptr<T>& obj) : ptr(const_cast<T*>(obj.get())) 
		{
			ptr->lock();
		}
		~AtomicPtr() 
		{
			ptr->unlock();
		}
		T* operator -> () {
			return ptr;
		}
	private:
		AtomicPtr(const AtomicPtr&);
		AtomicPtr& operator = (const AtomicPtr&);
	};

	template <class T>
	class Atomic {
		T obj;
		std::mutex lock;
		template< class _T > friend class AtomicPtr;

	public:
		Atomic(const T& t) : obj(t) {}
		Atomic() {}
	};
}
#endif 

