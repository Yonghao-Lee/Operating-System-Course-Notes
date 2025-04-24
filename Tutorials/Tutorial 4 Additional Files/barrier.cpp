#include "barrier.h"

Barrier::Barrier(int num_threads)
{
    this->arrived = 0; // number of threads that have arrived
    this->num_threads = num_threads; // total number of threads
}

void Barrier::wait()
{
	// acquire mutex to increment `arrived`
	std::unique_lock<std::mutex> lock(this->mtx);
	// now the mutex is locked
	int currently_arrived = ++this->arrived;
	if(currently_arrived == this->num_threads)
	{ // if all threads have arrived, notify all (broadcast)
		this->arrived = 0; // reset this->arrived
		this->cv.notify_all();
	}
	else
	{ // wait for broadcast, release the lock
		while(this->arrived != 0)
		{
			this->cv.wait(lock);
		}
	} // reaching to this point, the mutex is locked.
	// the mutex is automatically released since we used std::unique_lock
}