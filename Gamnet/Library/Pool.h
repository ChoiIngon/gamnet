/*
 * LockPool.h
 *
 *  Created on: 2013. 4. 25.
 *      Author: jjaehuny
 */

#ifndef __GAMNET_LIB_POOL_H_
#define __GAMNET_LIB_POOL_H_

#include <mutex>
#include <memory>
#include <deque>
#include <iostream>


namespace Gamnet {
	namespace Policy {
		/*!
		 * \brief policy for loking.
		 *
		 * 		  if use just one thread, use single_thread policy, or use multi_thread<br>
		 * 		  single_thread is a dummy lock implementing only interface
		 */

		struct nolock
		{
			void lock() {}
			bool try_lock() { return true; }
			void unlock() {}
		};

		namespace Initialze {
			struct do_nothing
			{
				template <class T>
				T* operator() (T* t)
				{
					return t;
				}
			};
		}; // Alloc
		namespace Factory
		{
			template <class T>
			struct create
			{
				T* operator() ()
				{
					return new T();
				}
			};
		};
	};

/*!
 * \brief Object Pool
 *
 * 		<ul>
 * 		<li>템플릿 인자로 지정된 타입의 객체를 내부적으로 생성하고 Create()함수로 그 포인터를 리턴한다.<br>
 * 		<li>reference counter를 사용하기 위해 std::shared_ptr를 기반으로 작성되어<br>
 * 			객체를 풀로 돌려주는 인터페이스는 따로 존재하지 않는다.
 * 		<li>객체는 필요시 마다 생성되어 삭제되지 않고 Pool에 저장되며<br>
 * 			생성자인자로 지정된 max 갯수만큼 이상을 생성하지는 못한다.
 *      </ul>
 *
 * \param object 풀이 생성/저장 할 객체 타입
 * \param lock_policy Toolkit::Policy::Lock 참조
 * \param init_policy 객체 초기화 정책<br>
 * 					    객체가 처음 생성되거나, 풀에 저장되어 있다 리턴되는 경우<br>
 * 					    멤버 변수 초기화 등을 어떻게 할 것인지 기술한다.<br>
 * 					  operator ()를 구현한 functor를 인자로 사용한다.<br>
 * <pre>
	struct sample_init_policy {
		template <class T>
		T* operator() (T* t) {
 *			//do something
			return t;
		}
	};
 * </pre>
 */
template<class object, class lock_policy = Policy::nolock, class init_policy=Policy::Initialze::do_nothing>
class Pool
{
	typedef std::function<object*()> object_factory;
	struct Deleter
	{
		typedef Pool<object, lock_policy, init_policy> pool_type;
		pool_type& pool_;
		Deleter(pool_type& pool) : pool_(pool)
		{
		}
		void operator()(void const* ptr) const
		{
			pool_.Restitute((const object*)ptr);
		}
	};
public:
	/*!
	 * \param nSize 풀이 생성 할수 있는 최대 오브젝트 갯수
	 * \param object_factory 객체 생성시 외부 인자를 참조하거나 할때 사용한다.
	 */
	Pool(int nSize = 65535, object_factory factory = Policy::Factory::create<object>()) : cur_size_(0), max_size_(nSize), factory_(factory)
	{
	}

	~Pool()
	{
		std::lock_guard<lock_policy> lo(lock_);
		for(auto itr : lstObjectPtr_)
		{
			delete itr;
		}
	}

	/*!
	 * \brief 객체 리턴
	 * \return std::shared_ptr<object> 형태 리턴
	 */
	std::shared_ptr<object> Create()
	{
		std::lock_guard<lock_policy> lo(lock_);
		if(0 >= lstObjectPtr_.size() && cur_size_ >= max_size_)
		{
			return std::shared_ptr<object>(NULL);
		}


		if(0 >= lstObjectPtr_.size())
		{
			cur_size_++;
			return std::shared_ptr<object>(init_(factory_()), Deleter(*this));
		}

		object* ptr = lstObjectPtr_.front();
		lstObjectPtr_.pop_front();
		return std::shared_ptr<object>(init_(ptr), Deleter(*this));
	}

	/*!
	 * \brief 현재 사용되지 않고 풀에 대기 중인 객체 카운터 리턴
	 */
	int Available() 
	{
		std::lock_guard<lock_policy> lo(lock_);
		return lstObjectPtr_.size() + (max_size_ - cur_size_);
	}

	int Size() 
	{
		std::lock_guard<lock_policy> lo(lock_);
		return cur_size_;
	}

	int Capacity() const
	{
		return max_size_;
	}
	void Capacity(int capacity)
	{
		max_size_ = capacity;
	}
private :
	void Restitute(const object* ptr)
	{
		std::lock_guard<lock_policy> lo(lock_);
		if(NULL == ptr)
		{
			return;
		}
		lstObjectPtr_.push_front(const_cast<object*>(ptr));
	}

private :
	lock_policy lock_;
	std::deque<object*> lstObjectPtr_;
	int cur_size_;
	int max_size_;
	init_policy init_;
	object_factory factory_;
};

} /*Toolkit*/

#endif /* TOOLKIT_LOCKPOOL_H_ */
