#ifndef NOX_MEMORY_LOCKEDALLOCATOR_H_
#define NOX_MEMORY_LOCKEDALLOCATOR_H_
#include <cstddef>
#include <mutex>

#include <nox/memory/Byte.h>

namespace nox
{
    namespace memory
    {
        /**
         * @brief      Basic lock wrapper for an allocator type. Basically just
         *             forwards all functions calls into the proper allocator,
         *             but additionally it locks with a mutex.
         *             LockAllocator is non-movable and non-copyable because of its mutex.
         *
         * @tparam     Allocator  requires 3 functions.
         *             void* allocate(std::size_t size);
         *             void deallocate(void* ptr);
         *             void clear();
         */
        template<class Allocator>
        class LockedAllocator
        {
        public:
            static constexpr std::size_t MAX_SIZE = Allocator::MAX_SIZE;

            LockedAllocator() = default;
            LockedAllocator(const LockedAllocator&) = delete;
            LockedAllocator& operator=(const LockedAllocator&) = delete;
            LockedAllocator(LockedAllocator&&) = delete;
            LockedAllocator& operator=(LockedAllocator&& source) = delete;
            ~LockedAllocator() = default;

            /**
             * @brief      Takes the mutex lock, and forwards to Allocator::allocate.
             */
            void* allocate(std::size_t size);
            
            /**
             * @brief       Takes the mutex lock, and forwards to Allocator::deallocate.
             */
            void deallocate(void* ptr);
            
            /**
             * @brief       Takes the mutex lock, and forwards to Allocator::clear. 
             */ 
            void clear();

        private:
            std::mutex mutex{};
            Allocator allocator{};
        };
    }
}

#include <nox/memory/LockedAllocator.tpp>

#endif
