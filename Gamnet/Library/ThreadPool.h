#ifndef __GAMNET_LIB_THREADPOOL_H_
#define __GAMNET_LIB_THREADPOOL_H_

#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <vector>

namespace Gamnet
{
/*!
 * \brief create thread objects
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
		Gamnet::ThreadPool tp(30);
		test_class tc;
		for(int i=0; i<10000000; i++)
		{
			tp.PostTask(std::bind(&test_func, i));
			tp.PostTask(std::bind(&test_class::func, &tc, i, i*2));
		}
		return 0;
	}
	</pre>
 */
class ThreadPool
{
private :
	struct Worker
	{
		ThreadPool& pool_;
		Worker(ThreadPool& pool);

		void operator () ();
	};
public:
	ThreadPool(int thread_count);
	virtual ~ThreadPool();
	/*!
	  \param t thread functor thread pool will execute<br>
			eg) std::bind(&some_function, arg1, arg2)
	 */
	template <class T>
	void PostTask(T t)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		tasks_.push_back(t);
		condition_.notify_one();
	}

	size_t GetTaskCount();
private :
	std::vector<std::thread > workers_;
	std::deque<std::function<void ()> > tasks_;
	bool stop_;
	std::mutex mutex_;
	std::condition_variable condition_;
};

}
#endif 
