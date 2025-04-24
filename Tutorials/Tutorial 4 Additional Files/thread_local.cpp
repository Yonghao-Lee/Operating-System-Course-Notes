#include <iostream>
#include <thread>

thread_local int x;

void thread_func(int tid)
{
    x = tid; 
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    // t2 probably already updated x
    // we would expect x to be 1
    std::cout << "x = " << x << std::endl;
}

int main()
{
    std::jthread t1(thread_func, 0);
    // t2 delays and updates x probably after t1
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    std::jthread t2(thread_func, 1);
    return 0;
}
