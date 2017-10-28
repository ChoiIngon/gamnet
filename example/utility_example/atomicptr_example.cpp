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


AtomicPtr<int> GetLockedObject()
{
	AtomicPtr<int> (atomic);
	return atomic;
}
int main() {
	std::thread t1(ThreadFunction);
	std::thread t2(ThreadFunction);
	std::thread t3(ThreadFunction);
	
	t1.join();
	t2.join();
	t3.join();

	AtomicPtr<int> ptr = GetLockedObject();
	return 0;
}
