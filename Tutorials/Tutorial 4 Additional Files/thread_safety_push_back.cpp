#include <iostream>
#include <vector>
#include <thread>

#define RANGE 1000000

void thread_function(std::vector<int> &shared, int range)
{
    for(int i = 0; i < range; i++)
    {
        shared.push_back(i);
    }
}

int main()
{
    // make sure you compile with -std=c++20 or later
    {
        std::jthread threads[8];
        std::vector<int> vec;
    
        for(int i = 0; i < 8; i++)
        {
            threads[i] = std::jthread(thread_function, std::ref(vec), RANGE);
        }
    }
    std::cout << "vec has " << vec.size() << " elements" << std::endl;
    return 0;
} 
