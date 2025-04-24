#include <iostream>
#include <thread>

void thread_func(int x, int y)
{
	std::cout << "x = " << x << " and y = " << y << std::endl;
}

int main(void)
{
	std::cout << "Creating Threads" << std::endl;
	std::thread t1(thread_func, 1, -1);
	std::thread t2(thread_func, 2, -2);
}
