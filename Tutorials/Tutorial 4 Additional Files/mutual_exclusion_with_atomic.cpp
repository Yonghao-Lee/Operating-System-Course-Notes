#include <iostream>
#include <thread>
#include <atomic>

std::atomic<int> x = 0;

void thread_function()
{
    for(int i = 0; i < 1000000; i++)
    {
        x++;
    }
}

int main(void)
{
    std::cout << "Creating Threads" << std::endl;
    std::thread t1(thread_function);
    std::thread t2(thread_function);
    t1.join();
    t2.join();
    std::cout << "x = " << x << std::endl;
    return 0;
}