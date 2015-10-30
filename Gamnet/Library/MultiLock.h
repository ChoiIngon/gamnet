#ifndef __GAMNET_LIB_MULTILOCK_H_
#define __GAMNET_LIB_MULTILOCK_H_

#include <mutex>
#include <memory>
#include <set>

template <class LOCK>
class MultiLock
{
private :
    std::set<LOCK*> setLocks;
public :
    template <class... LOCKS>
    MultiLock(LOCKS&&... locks) {
        Lock(locks...);

        for(auto lock : setLocks)
        {
            lock->lock();
        }
    }

    ~MultiLock()
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


#endif
