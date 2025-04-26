#include <iostream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include "pair_lock.h"

#define N 20

void thread_func(pair_lock &pl, std::mutex &mtx, int &inside, int tid)
{
    pl.lock();
    inside = 0;
    usleep(300);
    mtx.lock();
    int t = inside++;
    mtx.unlock();
    usleep(300);
    if(inside == 2)
    {
        if(t == 0) std::cout << "OK" << std::endl;
    }
    else
    {
        if(t == 0) std::cout << "FAIL - there are " << inside << " threads inside the critical section" << std::endl;
    }
    pl.release();
}

int main(int argc, char *argv[])
{
    pair_lock pl;
    std::mutex mtx;

    std::jthread threads[N];

    int inside = 0;
    for(int i = 0; i < N; i++)
    {
        threads[i] = std::jthread(thread_func, std::ref(pl), std::ref(mtx), std::ref(inside), i);
    }
    return 0;
}