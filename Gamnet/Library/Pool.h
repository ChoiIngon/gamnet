#ifndef __GAMNET_LIB_POOL_H_
#define __GAMNET_LIB_POOL_H_

#include <mutex>
#include <memory>
#include <deque>
#include <iostream>
#include <assert.h>
#include <functional>
namespace Gamnet {
	namespace Policy {
		/*!
		 * \brief policy for locking.
		 *
		 *		if need thread safe, use 'std::mutex' or 'std::recursive_mutex'
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
		}; 

		namespace Release {
			struct do_nothing
			{
				template <class T>
				T* operator() (T* t)
				{
					return t;
				}
			};
		};

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
 * 		<li>create 'object' type internally and returns std::shared_ptr of 'object' type<br>
 * 		<li>all created objects are managed by reference counter so no interface return the object to pool
 *      </ul>
 *
 * \param object pool will create this 'object' type instance
 * \param lock_policy Gamnet::Policy::lock, std::mutex or std::recursive_mutex
 * \param init_policy initialize policy for created object. <br>It will be called whenerver pool return object. It should implement 'operator ()'
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
template<class object, class lock_policy = Policy::nolock, class init_policy=Policy::Initialze::do_nothing, class release_policy=Policy::Release::do_nothing>
class Pool
{
	struct Deleter
	{
		typedef Pool<object, lock_policy, init_policy, release_policy> pool_type;
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
	typedef std::function<object*()> object_factory;
	/*!
	 * \param nSize max object count that this pool can create
	 * \param object_factory if you need custom create policy. you can use this
	 */
	Pool(int nSize = 65535, object_factory factory = Policy::Factory::create<object>(), init_policy init = init_policy(), release_policy release = release_policy()) : cur_size_(0), max_size_(nSize), init_(init), release_(release), factory_(factory)
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
	 * \brief return newly created or exist object
	 * \return std::shared_ptr<object>
	 */
	std::shared_ptr<object> Create()
	{
		std::lock_guard<lock_policy> lo(lock_);
		if(0 == lstObjectPtr_.size() && cur_size_ >= max_size_)
		{
			return nullptr;
		}

		if(0 == lstObjectPtr_.size())
		{
			object* ptr = factory_();
			if(nullptr == ptr)
			{
				return nullptr;
			}
			cur_size_++;
			std::shared_ptr<object> shared(init_(ptr), Deleter(*this));
			if(nullptr == shared)
			{
				lstObjectPtr_.push_front(ptr);
				return nullptr;
			}
			return shared;
		}

		object* ptr = lstObjectPtr_.front();
		assert(nullptr != ptr);
		std::shared_ptr<object> shared(init_(ptr), Deleter(*this));
		if (nullptr == shared)
		{
			return nullptr;
		}
		lstObjectPtr_.pop_front();
		return shared;
	}

	void SetFactory(object_factory factory)
	{
		std::lock_guard<lock_policy> lo(lock_);
		factory_ = factory;
	}
	/*!
	 * \brief return count that pool can create
	 */
	size_t Available() 
	{
		std::lock_guard<lock_policy> lo(lock_);
		return lstObjectPtr_.size() + (max_size_ - cur_size_);
	}

	/*!
	 * \brif return count that created or being used object
	 */
	size_t Size()
	{
		std::lock_guard<lock_policy> lo(lock_);
		return cur_size_;
	}

	size_t Capacity() const
	{
		return max_size_;
	}
	void Capacity(size_t capacity)
	{
		max_size_ = capacity;
	}
private :
	void Restitute(const object* ptr)
	{
		if (nullptr == ptr)
		{
			return;
		}

		std::lock_guard<lock_policy> lo(lock_);
		lstObjectPtr_.push_front(release_(const_cast<object*>(ptr)));
	}

private :
	lock_policy lock_;
	std::deque<object*> lstObjectPtr_;
	size_t cur_size_;
	size_t max_size_;
	init_policy init_;
	release_policy release_;
	object_factory factory_;
};

} 

#endif 
