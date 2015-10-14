/*
 * Atomic.h
 *
 *  Created on: Oct 13, 2015
 *      Author: wered
 */

#ifndef __GAMNET_LIB_ATOMIC_H_
#define __GAMNET_LIB_ATOMIC_H_

#include <mutex>

namespace Gamnet {
	template <class T>
	struct Atomic {
	    T obj;
	    std::mutex lock;
	    T operator = (const T& t) {
	        std::lock_guard<std::mutex> lo(lock);
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
}
#endif /* UTIL_ATOMIC_H_ */

