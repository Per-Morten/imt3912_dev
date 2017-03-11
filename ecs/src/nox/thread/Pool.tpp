#include <algorithm>
#include <chrono>

template<template<class> class QueueType>
nox::thread::Pool<QueueType>::Pool(std::size_t threadCount)
    : threads(threadCount)
{
    auto workerFunc = [this]() -> void
    {
        while (std::atomic_load_explicit(&this->shouldContinue, std::memory_order_seq_cst))
        {
            std::unique_lock<std::mutex> lock(this->cvMutex);
            Task task;
            
            // Continue waiting if: No tasks are popped from the queue, and we should continue.
            auto waitPred = [this, &task]() 
            { 
                return this->tasks.pop(task) || 
                       !std::atomic_load_explicit(&this->shouldContinue, 
                                                  std::memory_order_seq_cst); 
            };

            // Tells the thread to try and wake up again every  0.5 second to avoid "race"
            // on "registering" a wait on the cv.
            this->cv.wait_for(lock, 
                              std::chrono::milliseconds(500),
                              [waitPred]() { return waitPred(); });

            // No need to continue waiting after we have gotten the task.
            lock.unlock();

            // Task won't be valid if we exit because we are stopping the pool.
            if (task)
            {
                task();
                std::atomic_fetch_sub_explicit(&this->taskCount, 
                                               std::size_t(1), 
                                               std::memory_order_seq_cst);
            }
        }
    };

    for (auto& thread : this->threads)
    {
        thread = std::thread(workerFunc);
    }
}

template<template<class> class QueueType>
nox::thread::Pool<QueueType>::~Pool()
{
    std::atomic_store_explicit(&this->shouldContinue, 
                               false, 
                               std::memory_order_seq_cst);
    
    this->tasks.clear();
    this->cv.notify_all();

    for (auto& thread : this->threads)
    {
        thread.join();
    }
}

template<template<class> class QueueType>
void
nox::thread::Pool<QueueType>::addTask(const Task& task)
{
    std::atomic_fetch_add_explicit(&this->taskCount, 
                                   std::size_t(1), 
                                   std::memory_order_seq_cst);

    this->tasks.push(task);
    this->cv.notify_one();
}

template<template<class> class QueueType>
void
nox::thread::Pool<QueueType>::wait()
{
    while (std::atomic_load_explicit(&this->taskCount, std::memory_order_seq_cst) != 0)
    {
        this->cv.notify_all();
        std::this_thread::yield();    
    }
}

template<template<class> class QueueType>
void
nox::thread::Pool<QueueType>::clearTasks()
{
    std::atomic_store_explicit(&this->taskCount, 
                               std::size_t(1), 
                               std::memory_order_seq_cst);
    this->tasks.clear();
}

template<template<class> class QueueType>
std::size_t
nox::thread::Pool<QueueType>::threadCount() const
{
    return this->threads.size();
}
