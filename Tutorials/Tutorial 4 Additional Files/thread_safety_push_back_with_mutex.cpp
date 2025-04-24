#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#define RANGE 1000000

void thread_function(std::vector<int> &shared, int range, std::mutex &m)
{
    for(int i = 0; i < range; i++)
    {
        m.lock();
        shared.push_back(i);
        m.unlock();
    }
}

int main()
{
    // make sure you compile with -std=c++20 or later
    std::vector<int> vec;
    {
        std::jthread threads[8];
        std::mutex mutex;
    
        for(int i = 0; i < 8; i++)
        {
            threads[i] = std::jthread(thread_function, std::ref(vec), RANGE, std::ref(mutex));
        }

    }
    std::cout << "vec has " << vec.size() << " elements" << std::endl;
    return 0;
} 
