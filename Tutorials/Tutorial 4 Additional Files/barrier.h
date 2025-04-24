#include <mutex>
#include <condition_variable>

class Barrier
{
public:
	Barrier(int num_threads); // constructor
	
	void wait(); // wait for all threads to arrive 

private:
	int arrived;
	int num_threads;
	std::condition_variable cv;
	std::mutex mtx;
}; 
