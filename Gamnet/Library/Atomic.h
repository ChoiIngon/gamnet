#ifndef __GAMNET_LIB_ATOMIC_H_
#define __GAMNET_LIB_ATOMIC_H_

#include <mutex>
#include <memory>

namespace Gamnet {
	template <class T> class AtomicPtr;
	template <class T>
	class Atomic {
		T obj;
		std::mutex lock;
		template< class _T > friend class AtomicPtr;

	public:
		Atomic(const T& t) : obj(t) {}
		Atomic() {}
	};

	template <class T>
	class AtomicPtr {
		Atomic<T>* ptr;
	public:
		AtomicPtr(const Atomic<T>& obj) : ptr(const_cast<Atomic<T>*>(&obj)) {
			ptr->lock.lock();
		}
		AtomicPtr(const Atomic<T>* obj) : ptr(const_cast<Atomic<T>*>(obj)) {
			ptr->lock.lock();
		}
		AtomicPtr(const std::shared_ptr<Atomic<T>>& obj) : ptr(const_cast<Atomic<T>*>(obj.get())) {
			ptr->lock.lock();
		}
		
		~AtomicPtr() {
			ptr->lock.unlock();
		}
		T& operator * () {
			return ptr->obj;
		}
		T* operator -> () {
			return &(ptr->obj);
		}
	private:
		AtomicPtr(const AtomicPtr&);
		AtomicPtr& operator = (const AtomicPtr&);
	};
}
#endif 

