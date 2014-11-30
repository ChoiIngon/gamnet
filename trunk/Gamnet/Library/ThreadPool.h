#ifndef __GAMNET_LIB_THREADPOOL_H_
#define __GAMNET_LIB_THREADPOOL_H_

#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <vector>

/*!
 * \file ThreadPool.h
 */
namespace Gamnet
{
/*!
 * \brief 지정된 갯수 만큼 스레드 생성, task 할당 및 라이프 사이클 관리
 *
 * 		 ThreadPool은 작업 스레드(worker thread)와 작업 큐(task queue)를 미리 생성해 두고,<br>
 * 		  큐에 요청 되는 작업들을 pool 내에 스레드에게 할당하며, <br>
 * 		  해당 작업이 완료되어 리턴하면 다시 스레드를 pool에서 대기 시킨다.<br>
 *
 * 		 Usage :
 * 		 <ol>
 * 		 <li>create "ThreadPool" object with the number specifying how many thread you need
 * 		 <li>create "function" or "functor" to process your tasks
 * 		 <li>post your tasks("function" or "functor" above) to the "ThreadPool" object
 * 		 </ol>
 * 		 Remark :<br>
 * 		 <ul>
 * 		 <li>build with "-lpthread"<br>
 * 		 </ul>
 * 		 Sample code :<br>
 * 		 <pre>
 	void test_func(int i)
	{
		std::cout << i << std::endl;
	}

	struct test_class
	{
		void func(int i, int j)
		{
			std::cout << i << " " << j << std::endl;
		}
	};

	int main()
	{
		Toolkit::ThreadPool tp(30);
		test_class tc;
		for(int i=0; i<10000000; i++)
		{
			tp.PostTask(std::bind(&test_func, i));
			tp.PostTask(std::bind(&test_class::func, &tc, i, i*2));
		}
		return 0;
	}
	</pre>

 * \author : kukuta
 * \version : 1.3
 */
class ThreadPool
{
private :
	struct Worker
	{
		ThreadPool& pool_;
		Worker(ThreadPool& pool)
			: pool_(pool)
		{
		}

		void operator () ()
		{
			while(true)
			{
				std::function<void()> f;
				{
					std::unique_lock<std::mutex> lock(pool_.mutex_);
					while(false == pool_.stop_ && pool_.tasks_.empty())
					{
						pool_.condition_.wait(lock);
					}

					if(true == pool_.stop_)
					{
						return;
					}

					f = pool_.tasks_.front();
					pool_.tasks_.pop_front();
				}
				f();
			}
		}
	};
public:
	/*!
	  \param thread_count pool 내에 생성될 thread의 갯수
	 */
	ThreadPool(int thread_count)
		: stop_(false)
	{
		for(int i=0; i<thread_count; i++)
		{
			workers_.push_back(std::thread(Worker(*this)));
		}
	}
	virtual ~ThreadPool()
	{
		stop_ = true;
		condition_.notify_all();

		std::for_each (workers_.begin(), workers_.end(), [](std::thread& thr) {
			thr.join();
		});
	}
	/*!
	  \param t thread pool에서 실행 하게 될 함수자(functor)<br>
                            ※ std::bind(&some_function, arg1, arg2)
	 */
	template <class T>
	void PostTask(T t)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		tasks_.push_back(t);
		condition_.notify_one();
	}
private :
	std::vector<std::thread > workers_;
	std::deque<std::function<void ()> > tasks_;
	bool stop_;
	std::mutex mutex_;
	std::condition_variable condition_;
};

}
#endif /* THREADPOOL_H_ */
