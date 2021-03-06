#ifndef NOX_THREAD_POOL_H_
#define NOX_THREAD_POOL_H_
#include <atomic>
#include <condition_variable>
#include <thread>
#include <functional>

namespace nox
{
    namespace thread
    {
        /**
         * @brief      Thread pool used within ECS. Currently interface is
         *             pretty limited.
         *
         * @todo make template with container type.
         *
         * @tparam     QueueType  QueueType must be a thread safe queue with the
         *                        following functions:
         *
         *             bool pop(reference value), returning true if a value was
         *             popped successfully.
         *
         *             void clear(), removes all elements.
         *
         *             void push(const reference value), pushes value onto the
         *             back of the queue.
         */
        template<template <class> class QueueType>
        class Pool
        {
        public:
            /**
             * @brief      Currently the task is just a std::function, might be
             *             turned into a regular function ptr or something in
             *             the future. Depends on how it integrates with the
             *             EntityManager.
             */
            using Task = std::function<void(void)>;

            /**
             * @brief      Creates a pool with the given number of threads. If
             *             no parameters are given, the pool asks the OS how
             *             many threads are available and creates that amount,
             *             minimum one.
             *
             * @param[in]  threadCount  The number of threads in the pool.
             */
            Pool(std::size_t threadCount = std::max(std::thread::hardware_concurrency(), 2u) - 1u);

            /**
             * @brief      Copy constructing thread pool is illegal because of the
             *             non-copyable types.
             */
            Pool(const Pool&) = delete;

            /**
             * @brief      Copy assigning thread pool is illegal because of the
             *             non-copyable types.
             */
            Pool& operator=(const Pool&) = delete;

            /**
             * @brief      Move constructing thread pool is illegal because of the
             *             non-movable types.
             */
            Pool(Pool&&) = delete;

            /**
             * @brief      Move assigning thread pool is illegal because of the
             *             non-movable types.
             */
            Pool& operator=(Pool&&) = delete;

            /**
             * @brief      Notifies all threads that they should stop execution,
             *             before joining the threads. All tasks left in the
             *             queue is discarded.
             */
            ~Pool();

            /**
             * @brief      Adds a task to the queue of tasks to be finished.
             *
             * @param[in]  task  The task to run.
             */
            void addTask(const Task& task);

            /**
             * @brief      Removes all the tasks from the taskQueue.
             */
            void clearTasks();

            /**
             * @brief      Blocking function, finishes running all the functions in the queue.
             */
            void wait();

            /**
             * @brief      Returns the number of threads this pool has.
             *
             * @return     Number of threads belonging to this pool.
             */
            std::size_t threadCount() const;

        private:
            std::condition_variable cv{};
            std::mutex cvMutex{};
            QueueType<Task> tasks;
            std::vector<std::thread> threads{};

            /**
             * @brief      Indicates how many tasks are left to processed, used
             *             within the wait function to block the calling thread
             *             properly.
             *
             *             Memory Order: Task count only does acquire release
             *             order, as we don't think we need sequentially
             *             consistency. We think it could have been relaxed as
             *             we are mostly doing opaque function calls, however,
             *             we choose to be on the safe side and go for acquire
             *             release.
             */
            std::atomic<std::size_t> taskCount{0};

            /**
             * @brief      Used to indicate whether or not a thread should
             *             continue with tasks, or if the pool is stopping.
             *
             *             Memory Order: All ordering is relaxed, we are not
             *             guarding any information with this variable, and it
             *             is only used for signaling to stop in the future.
             */
            std::atomic<bool> shouldContinue{true};
        };
    }
}

#include <nox/thread/Pool.tpp>
#endif
