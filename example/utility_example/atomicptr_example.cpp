#include "../../Gamnet/Library/Atomic.h"
#include <iostream>
#include <thread>

	Gamnet::Atomic<int> atomic(0);
// 1. delclear Atomic<T> variable

void ThreadFunction()
{
		for(int i= 0; i<10; i++)
		{
			Gamnet::AtomicPtr<int> ptr(atomic);
			(*ptr)++;
			std::cout << (*ptr) << std::endl;
		}
}

int main() {
	Gamnet::Atomic<std::shared_ptr<int>> shared_atomic;
	Gamnet::AtomicPtr< std::shared_ptr<int> > ptr(shared_atomic);

	std::thread t1(ThreadFunction);
	std::thread t2(ThreadFunction);
	std::thread t3(ThreadFunction);
	
	t1.join();
	t2.join();
	t3.join();
	return 0;
}
