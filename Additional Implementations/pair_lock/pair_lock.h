#ifndef PAIR_LOCK_H
#define PAIR_LOCK_H

// Include necessary standard library headers.
#include <thread> // Although not directly used here, often needed when working with threads.
#include <mutex>  // Provides std::mutex and std::unique_lock
#include <condition_variable> // Provides std::condition_variable

// Defines the pair_lock class.
class pair_lock
{
public: // Public interface accessible to users of the class.
    /*
        Constructor declaration.
    */
    pair_lock(void);

    /*
        Lock method declaration.
        Users call this to enter the paired critical section.
        It ensures only pairs enter and only one pair at a time.
    */
    void lock(void);

    /*
        Release method declaration.
        Users call this when exiting the paired critical section.
        It ensures both partners synchronize on exit before releasing the lock.
    */
    void release(void);

private: // Internal implementation details, not directly accessible by users.
    // Mutex: The primary lock protecting ALL shared member variables below.
    // Any access to arrive_count_, release_count_, in_critical_section_,
    // or use of the condition variables MUST happen while holding this mutex.
    std::mutex mtx_;

    // Condition Variable for Arrival Pairing:
    // The first thread arriving at lock() waits on this until the second arrives.
    std::condition_variable cv_arrive_;

    // Condition Variable for Release Synchronization:
    // The first thread arriving at release() waits on this until the second arrives.
    std::condition_variable cv_release_;

    // Condition Variable for Overall Lock Availability:
    // Threads arriving at lock() wait on this if another pair is already
    // inside the critical section (i.e., if in_critical_section_ is true).
    std::condition_variable cv_next_pair_;

    // Counter for threads arriving at lock() trying to form a pair (0, 1, or 2).
    int arrive_count_;

    // Counter for threads arriving at release() trying to sync up (0, 1, or 2).
    int release_count_;

    // Flag indicating if a pair is currently executing the critical section.
    // This enforces mutual exclusion between pairs.
    bool in_critical_section_;
}; // End of class definition

// Implementation of the constructor.
pair_lock::pair_lock(void)
{
    // Initialize the arrival counter to 0 (no threads waiting to pair).
    arrive_count_ = 0;
    // Initialize the release counter to 0 (no threads waiting to release).
    release_count_ = 0;
    // Initialize the lock state to 'not occupied'.
    in_critical_section_ = false;
}

// Implementation of the lock method.
void pair_lock::lock(void)
{
    // Create a unique_lock object 'lk'. This immediately attempts to lock the mutex 'mtx_'.
    // If locked by another thread, it waits here.
    // The lock is automatically released when 'lk' goes out of scope (RAII).
    std::unique_lock<std::mutex> lk(mtx_);

    // --- Gate Check ---
    // Check if a pair is already inside the critical section.
    // Use 'while' loop to handle spurious wakeups correctly. Even if woken,
    // the thread must re-check if in_critical_section_ is still true.
    while (in_critical_section_) {
        // If lock is busy, wait on the 'cv_next_pair_' condition variable.
        // IMPORTANT: wait() atomically releases the mutex 'lk' before sleeping
        // and re-acquires it before waking up / returning. This allows other
        // threads (specifically the pair inside calling release()) to acquire
        // the mutex and change in_critical_section_.
        cv_next_pair_.wait(lk);
    }
    // If we pass this loop, in_critical_section_ is false (the lock is free).

    // --- Arrival Pairing Logic ---
    // Increment the count of threads arriving to form a pair.
    // This is safe because we hold the lock 'lk'.
    arrive_count_++;

    // Check if this is the first thread of the pair (count is 1)
    // or the second thread (count is 2).
    if (arrive_count_ < 2) { // arrive_count_ is now 1
        // This is the first thread of the pair. It must wait for the second.
        // Wait on the 'cv_arrive_' condition variable.
        // The lambda `[this] { return arrive_count_ == 0; }` is the wait predicate.
        // The thread will only wake up AND proceed if:
        //  1. It receives a notification on cv_arrive_.
        //  2. AND the predicate (arrive_count_ == 0) evaluates to true after waking
        //     and re-acquiring the lock 'lk'.
        // This predicate handles spurious wakeups and ensures the second thread
        // has not only arrived but also reset the counter before this thread proceeds.
        cv_arrive_.wait(lk, [this] { return arrive_count_ == 0; });
        // When wait() returns, this first thread knows its partner has arrived
        // and signaled, and it holds the lock 'lk'. It can now proceed.

    } else { // arrive_count_ is now 2
        // This is the second thread of the pair.
        // The pair is now complete and can enter the critical section.
        // Mark the lock as occupied by this pair.
        in_critical_section_ = true;
        // Reset the arrival counter for the *next* pair that will form later.
        // Crucially, reset it *before* notifying the waiting partner, because
        // the partner's wait predicate checks for arrive_count_ == 0.
        arrive_count_ = 0;
        // Notify the first thread (which is waiting on cv_arrive_) that its
        // partner has arrived and the state is ready.
        cv_arrive_.notify_one();
    }
    // Both threads (the first woken, the second continuing) exit the lock() function here.
    // 'lk' goes out of scope, releasing the mutex 'mtx_'.
    // They can now proceed into the user's critical section code.
}

// Implementation of the release method.
void pair_lock::release(void)
{
    // Acquire the main mutex 'mtx_' to safely modify shared state.
    std::unique_lock<std::mutex> lk(mtx_);

    // Increment the count of threads arriving at the release point.
    release_count_++;

    // Check if this is the first thread of the pair to call release (count is 1)
    // or the second (count is 2).
    if (release_count_ < 2) { // release_count_ is now 1
        // This is the first thread to finish. It must wait for its partner.
        // Wait on the 'cv_release_' condition variable.
        // The predicate `[this] { return release_count_ == 0; }` ensures it
        // waits until the second thread has arrived and reset the counter.
        // This handles spurious wakeups and ensures correct synchronization.
        cv_release_.wait(lk, [this] { return release_count_ == 0; });
        // When wait() returns, this first thread knows its partner has also
        // called release() and signaled. It holds the lock 'lk'.

    } else { // release_count_ is now 2
        // This is the second thread of the pair to call release.
        // Both partners have now indicated they are finished.
        // Reset the release counter for the next pair. Do this before notifying.
        release_count_ = 0;
        // Mark the critical section as no longer occupied / free the lock.
        in_critical_section_ = false;
        // Notify *all* threads that might be waiting at the start of lock()
        // (on cv_next_pair_) that the lock is now free. They will wake up,
        // re-acquire the mutex one by one, and re-check the 'while' loop condition.
        // Using notify_all() ensures that waiting threads get a chance to compete
        // to form the next pair.
        cv_next_pair_.notify_all();
        // Notify the first thread of *this pair* (which is waiting on cv_release_)
        // that its partner has arrived at release().
        cv_release_.notify_one();
    }
    // Both threads (the first woken, the second continuing) exit the release() function.
    // 'lk' goes out of scope, releasing the mutex 'mtx_'.
    // The lock is now fully released and available.
}

// End of the header guard block.
#endif //PAIR_LOCK_H