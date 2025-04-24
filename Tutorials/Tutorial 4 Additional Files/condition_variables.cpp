#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

int x = 0;
std::condition_variable cv;
std::mutex mtx;

void thread1(void)
{
    std::unique_lock<std::mutex> lock(mtx);
    // do some calculations
    x++;
    // do some calculations
    std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate work
    x++;
    // now thread2 continues
    cv.notify_all();
}

void thread2(void)
{
    std::unique_lock<std::mutex> lock(mtx);
    while(x != 2)
    {
        cv.wait(lock);
    }
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