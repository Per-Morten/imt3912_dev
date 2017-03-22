#ifndef NOX_MEMORY_LINEARALLOCATOR_H_
#define NOX_MEMORY_LINEARALLOCATOR_H_
#include <cstdint>
#include <nox/memory/Byte.h>

namespace nox
{
    namespace memory
    {
        /**
         * @brief      A Linear allocator that allocates memory blocks with
         *             slotCount number of slots of slotSize size. Memory is
         *             freed upon destruction.
         *
         * @tparam     blockSize  How big each allocated block is.
         */
        template<std::size_t blockSize>
        class LinearAllocator
        {
        public:
            
            static constexpr std::size_t MAX_SIZE = blockSize;

            /**
             * @brief      Creates the LinearAllocator with as many blocks as
             *             specified by the parameter.
             *
             * @param[in]  initialBlockCount  The initial number of blocks. Must be > 0.
             */
            LinearAllocator(std::size_t initialBlockCount = 1);

            /**
             * @brief      Type is only movable.
             */
            LinearAllocator(const LinearAllocator&) = delete;
            
            /**
             * @brief      Type is only movable.
             */
            LinearAllocator& operator=(const LinearAllocator&) = delete;
            
            /**
             * @brief      Move constructor. Moves all members out of source.
             *
             * @warning    source will not be usable after moving. 
             * 
             * @param[in]  source  the LinearAllocator to move from.
             */
            LinearAllocator(LinearAllocator&& source);

            /**
             * @brief      Move assignment operator. Moves all members out of
             *             source. The class is tolerant of self-assignment.
             *
             * @warning    source will not be usable after moving.
             *
             * @param[in]  source  the LinearAllocator to move from.
             *
             * @return     *this after assignment.
             */
            LinearAllocator& operator=(LinearAllocator&& source);
            
            /**
             * @brief      Destroys all blocks in the pool. The user must ensure
             *             that none of the slots holds any non destructed
             *             objects.
             */
            ~LinearAllocator();

            /**
             * @brief      Allocates a slotSize sized memory area. The function
             *             ignores the parameter, the only reason for this
             *             parameter existing is to ease performance testing in
             *             real world case. See:
             *             https://github.com/Per-Morten/imt3912_dev/issues/81
             *
             * @param[in]  size  Ignored!
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
             * @brief      Resets all the slots to 0. User must ensure that
             *             delete has been called on all stored elements first.
             *             Memory is not freed until destruction.
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
                 * @brief      How many bytes that is used within the slots.
                 */
                std::size_t used{};

                /**
                 * @brief      Area of raw memory which is allocated into.
                 */
                Byte slots[MAX_SIZE];
            };

            /**
             * @brief Pointer to the first block within the list.
             */
            Block* first{};

            /**
             * @brief Pointer to the first block that contains free slots.
             */
            Block* firstFree{};
        };
    }
}

#include <nox/memory/LinearAllocator.tpp>

#endif
