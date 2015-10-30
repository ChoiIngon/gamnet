/*
 * Atomic.h
 *
 *  Created on: Oct 13, 2015
 *      Author: wered
 */

#ifndef __GAMNET_LIB_ATOMIC_H_
#define __GAMNET_LIB_ATOMIC_H_

#include <mutex>
#include <memory>

namespace Gamnet {
	template <class T>
	struct Atomic {
	    T obj;
	    std::recursive_mutex lock;
	    T operator = (const T& t) {
	        std::lock_guard<std::recursive_mutex> lo(lock);
	        obj = t;
	        return obj;
	    }
	};
	
	template <class T>
	class AtomicPtr {
	    Atomic<T>* ptr;
	public :
	    AtomicPtr(const Atomic<T>& obj) {
	        ptr = const_cast<Atomic<T>*>(&obj);
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
	private :
	    AtomicPtr(const AtomicPtr&);
	    AtomicPtr& operator = (const AtomicPtr&);
	};

	template <class T>
	class AtomicPtr<std::shared_ptr<T>> {
	    Atomic<std::shared_ptr<T>>& ptr;
	public :
	    AtomicPtr(const Atomic<std::shared_ptr<T>>& obj) {
	        ptr = const_cast<Atomic<std::shared_ptr<T>>*>(&obj);
	        ptr->lock.lock();
	    }
	    ~AtomicPtr() {
	        ptr->lock.unlock();
	    }
	    T& operator * () {
	        return *(ptr->obj);
	    }
	    std::shared_ptr<T> operator -> () {
	        return ptr->obj;
	    }
	};
}
#endif /* UTIL_ATOMIC_H_ */

