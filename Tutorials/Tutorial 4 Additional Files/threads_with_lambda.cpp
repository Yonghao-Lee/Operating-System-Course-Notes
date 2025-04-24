#include <iostream>
#include <thread>
#include <unistd.h>

void thread_func(int x, int y)
{
	std::cout << "x = " << x << " and y = " << y << std::endl;
}

int main(void)
{
    // make sure you compile with -std=c++20 or later
    int x = 0;
    std::thread t([](const int &wait)
                    {sleep(wait);} , 5);
    
    t.join();
    std::cout << "Out!" << std::endl;
}

