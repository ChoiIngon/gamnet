#ifndef __GAMNET_LIB_MULTILOCK_H_
#define __GAMNET_LIB_MULTILOCK_H_

#include <mutex>
#include <memory>
#include <set>

template <class LOCK>
class [[deprecated("use std::scoped_lock(C++17) instead")]] MultiLock
{
    class MultiLockImpl
    {
    private :
        std::set<LOCK*> setLocks;
    public :
        template <class... LOCKS>
        MultiLockImpl(LOCKS&&... locks) {
            Lock(locks...);
            for(auto lock : setLocks)
            {
                lock->lock();
            }
        }

        ~MultiLockImpl()
        {
        	for(auto lock : setLocks)
            {
                lock->unlock();
            }
        }

    private :
        template <class T, class... LOCKS>
        void Lock(T& lock, LOCKS&... locks)
        {
            setLocks.insert(&lock);
            Lock(locks...);
        }
        template <class T>
        void Lock(T& lock)
        {
            setLocks.insert(&lock);
        }
    };
    std::shared_ptr<MultiLockImpl> impl_;
public :
    template <class... LOCKS>
    MultiLock(LOCKS&&... locks) {
        impl_ = std::shared_ptr<MultiLockImpl>(new MultiLockImpl(locks...));
    }

    ~MultiLock()
    {
    }
};


#endif
