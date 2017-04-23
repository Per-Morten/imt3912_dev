#ifndef NOX_MEMORY_LOCKFREEALLOCATOR_H_
#define NOX_MEMORY_LOCKFREEALLOCATOR_H_
#include <atomic>
#include <cstddef>

#include <nox/memory/Byte.h>

namespace nox
{
    namespace memory
    {
        /**
         * @brief      A thread safe linear allocator with certain lock-free
         *             functionality. The allocator is a linear allocator,
         *             meaning that memory cannot be deallocated on the fly, but
         *             has to be deallocated through the clear function. Memory
         *             is allocated from a linked-list containing smaller memory
         *             pools, allowing for semi contiguous memory. The allocator
         *             can suffer from internal fragmentation, and should only
         *             be used for short lived allocations.
         *
         * @note       Currently the allocator does not support functionality
         *             for deallocating these pools, this should be added in the
         *             future.
         *
         * @tparam     blockSize  The size of each memory block within each
         *                        element of the list. i.e. How much memory to
         *                        allocate into within each element of the list.
         */
        template<std::size_t blockSize>
        class LockFreeAllocator
        {
        public: 
            /**
             * @brief      The maximum size of one allocation.
             */
            static constexpr std::size_t MAX_SIZE = blockSize;

            /**
             * @brief      Creates the LockFreeAllocator with as many blocks as
             *             specified by the parameter.
             *
             * @param[in]  initialBlockCount  The initial number of blocks. Must be > 0.
             */
            LockFreeAllocator(std::size_t initialBlockCount = 1);

            /**
             * @brief      Type is not copyable.
             */
            LockFreeAllocator(const LockFreeAllocator&) = delete;
            
            /**
             * @brief      Type is not copyable.
             */
            LockFreeAllocator& operator=(const LockFreeAllocator&) = delete;
            
            /**
             * @brief      Type is not movable.
             */
            LockFreeAllocator(LockFreeAllocator&&) = delete;

            /**
             * @brief      Type is not movable.
             */
            LockFreeAllocator& operator=(LockFreeAllocator&& source) = delete;
            
            /**
             * @brief      Destroys all blocks in the pool. The user must ensure
             *             that none of the slots holds any non destructed
             *             objects.
             */
            ~LockFreeAllocator();

            /**
             * @brief      Allocates size bytes of uninitialized storage.
             *             Function can be called concurrently.
             *
             * @param[in]  size  The size of the memory to allocate, in bytes.
             *                   size must satisfy: 0 < size <= MAX_SIZE.
             *
             * @return     Pointer to allocated uninitialized memory.
             */
            void* allocate(std::size_t size);
            
            /**
             * @brief      Function is ignored in this allocator. The function
             *             exists only reason for easing performance testing in
             *             real world case. See:
             *             https://github.com/Per-Morten/imt3912_dev/issues/81
             *
             * @param      ptr   Ignored!
             */
            void deallocate(void* ptr) {}
            
            /**
             * @brief      Prepares all elements for reuse. User must ensure
             *             that the destructor has been run on all stored
             *             elements first. Memory is not freed until
             *             destruction.
             *
             *             Function cannot be called concurrently. As it could
             *             result in memory corruption.
             */ 
            void clear();

        private:
            /**
             * @brief      Block holding a link to the next block, as well as
             *             the memory area within the block.
             */ 
            struct Block
            {
                /**
                 * @brief      Pointer to next block within the list of blocks.
                 */
                Block* next{};

                /**
                 * @brief      How many bytes that is used within the memory.
                 */
                std::atomic<std::size_t> used{};

                /**
                 * @brief      Area of raw memory which is allocated into.
                 */
                Byte memory[MAX_SIZE]{0};
            };

            /**
             * @brief      Tries to allocate size bytes of uninitialized storage
             *             within the memory of the block.
             *
             * @param      block  The block to try and allocate into.
             * @param      size   The size in bytes to allocate.
             *
             * @return     Pointer to uninitialized storage within the memory of
             *             block if success, nullptr if the allocation fails.
             */
            void* tryAllocate(Block& block, std::size_t size);

            /**
             * @brief      Pointer to the first block within the list.
             *             Kept around only so we can do cleanup and clears.
             */
            Block* first{};

            /**
             * @brief      Pointer to the first block that contains free slots.
             */
            std::atomic<Block*> firstFree{};
        };
    }
}

#include <nox/memory/LockFreeAllocator.tpp>

#endif
