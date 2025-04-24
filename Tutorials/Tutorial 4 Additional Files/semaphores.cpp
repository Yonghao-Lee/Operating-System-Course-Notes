#include <iostream>
#include <thread>
#include <semaphore>

int x = 0;
std::counting_semaphore sem(0); 

void thread1(void)
{
    // do some calculations
    x++;
    // do some calculations
    std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate work
    x++;
    // now thread2 continues
    sem.release();
}

void thread2(void)
{
    sem.acquire();
    std::cout << "Thread 2: x = " << x << std::endl;
}

int main(void)
{
    std::thread t1(thread1);
    std::thread t2(thread2);

    t1.join();
    t2.join();

    return 0;
}