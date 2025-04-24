#include <iostream>
#include <thread>

void thread_func(int x, int y)
{
	std::cout << "x = " << x << " and y = " << y << std::endl;
}

int main(void)
{
    // make sure you compile with -std=c++20 or later
	std::cout << "Creating Threads" << std::endl;
	std::jthread t1(thread_func, 1, -1);
	std::jthread t2(thread_func, 2, -2);
}
