#ifndef NOX_THREAD_THREADSAFEQUEUE_H_
#define NOX_THREAD_THREADSAFEQUEUE_H_
#include <mutex>
#include <queue>

namespace nox
{
    namespace thread
    {
        /**
         * @brief      Locked queue with a minimalistic interface.
         *
         * @detail     The queue is a locked queue with a minimalistic interface 
         *             The rationale for this is that when we are thinking of 
         *             doing a lock free queue this minimalistic interface 
         *             might be the only thing that we can really support.
         *
         * @tparam     T can be of any copyable type.
         */
        template<class T>
        class LockedQueue
        {
        public:
            /**
             * @brief      Creating type aliases to allow the users to use regular typedefs.
             */
            using container_type = std::queue<T>;
            using value_type = typename container_type::value_type;
            using size_type = typename container_type::size_type;
            using reference = typename container_type::reference;
            using const_reference = typename container_type::const_reference;

            /**
             * @brief      Default constructor setting up the queue. 
             *             No other constructor functionality is offered 
             *             to keep the interface minimalistic.
             */ 
            LockedQueue() = default;

            /**
             * @brief      Copying locked queues are illegal because of the
             *             mutexes.
             */
            LockedQueue(const LockedQueue&) = delete;

            /**
             * @brief      Copy assigning locked queues are illegal because of
             *             mutexes.
             */
            LockedQueue& operator=(const LockedQueue&) = delete;

            /**
             * @brief      Move constructing locked queues are illegal because
             *             of mutexes.
             */
            LockedQueue(LockedQueue&& other) = delete;

            /**
             * @brief      Move assigning locked queues are illegal because of
             *             mutexes.
             */
            LockedQueue& operator=(LockedQueue&& other) = delete;

            /**
             * @brief      Destroys the object.
             */
            ~LockedQueue() = default;

            /**
             * @brief      Pushes a value onto the queue.
             *
             * @param[in]  value  The value
             */
            void push(const_reference value);

            /**
             * @brief         Pops a value of the queue if possible and stores
             *                it in value. If no value can be popped from queue
             *                "value" remains unchanged.
             *
             * @param[out] value  The value to store the popped value from
             *                       the queue in.
             *
             * @return        True if a value could be popped from the queue.
             *                False otherwise.
             */
            bool pop(reference value);

            /**
             * @brief      Removes all elements from the queue.
             */
            void clear();

            /**
             * @brief      Returns the number of elements within the queue.
             *
             * @return     Number of elements in the queue.
             */
            size_type size() const;

            /**
             * @brief      Checks if the queue is empty.
             *
             * @return     True of the container is empty, false otherwise.
             */
            bool empty() const;

        private:
            std::queue<T> queue;
            mutable std::mutex mutex;
        };
    }
}

#include <nox/thread/LockedQueue.tpp>

#endif
