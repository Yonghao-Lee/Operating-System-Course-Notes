#include <iostream>
#include <thread>
#include <mutex>

void thread_func(std::mutex &m, int thread_num)
{
    std::unique_lock<std::mutex> lock(m); // protects the printout
    std::cout << "I am thread " << thread_num << std::endl;
}

int main()
{
    std::mutex m;
    std::jthread threads[10];
    for(int i = 0; i < 10; i++)
    {
        threads[i] = std::jthread(thread_func, std::ref(m), i);
    }
}
