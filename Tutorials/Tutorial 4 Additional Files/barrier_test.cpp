#include <thread>
#include <iostream>
#include "barrier.h"

#define NUM_THREADS 5

int main(int argc, char *argv[])
{
    auto thread_function = [](int i, Barrier &barrier)
    {
        std::this_thread::sleep_for(std::chrono::seconds(i));
        std::cout << "Thread " << i << " is waiting at the barrier." << std::endl;
        barrier.wait();
        std::cout << "Thread " << i << " has passed the barrier." << std::endl;
    };

    Barrier barrier(NUM_THREADS);
    std::thread threads[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; ++i)
    {
        threads[i] = std::thread(thread_function, i, std::ref(barrier));
    }
    for(std::thread &t : threads)
    {
        t.join();
    }
}