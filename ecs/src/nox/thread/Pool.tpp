#include <algorithm>
#include <chrono>

template<template<class> class QueueType>
nox::thread::Pool<QueueType>::Pool(std::size_t threadCount)
    : threads(threadCount)
{
    auto workerFunc = [this]() -> void
    {
        while (this->shouldContinue.load(std::memory_order_relaxed))
        {
            std::unique_lock<std::mutex> lock(this->cvMutex);
            Task task{};

            // Stop waiting if: A task is popped successfully from the queue or we should not continue.
            auto waitPred = [this, &task]()
            {
                return this->tasks.pop(task) ||
                       !this->shouldContinue.load(std::memory_order_relaxed);
            };

            // Tells the thread to try and wake up again every  0.25 second to avoid "race"
            // on "registering" a wait on the cv, and to ensure that all threads wake up at some point
            // in the destructor of the pool.
            this->cv.wait_for(lock,
                              std::chrono::milliseconds(250),
                              [waitPred]() { return waitPred(); });

            // No need to continue waiting after we have gotten the task.
            lock.unlock();

            // Task won't be valid if we exit because we are stopping the pool.
            if (task)
            {
                task();
                this->taskCount.fetch_sub(1, std::memory_order_release);
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
    this->shouldContinue.store(false, std::memory_order_relaxed);
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
    this->tasks.push(task);
    this->taskCount.fetch_add(1, std::memory_order_release);

    this->cv.notify_one();
}

template<template<class> class QueueType>
void
nox::thread::Pool<QueueType>::clearTasks()
{
    this->tasks.clear();
    this->taskCount.store(0, std::memory_order_release);
}

template<template<class> class QueueType>
void
nox::thread::Pool<QueueType>::wait()
{
    while (this->taskCount.load(std::memory_order_acquire) != 0)
    {
        this->cv.notify_all();
        std::this_thread::yield();
    }
}

template<template<class> class QueueType>
std::size_t
nox::thread::Pool<QueueType>::threadCount() const
{
    return this->threads.size();
}
