#ifndef NOX_MEMORY_LOCKEDPOOL_H_
#define NOX_MEMORY_LOCKEDPOOL_H_
#include <cstdint>
#include <nox/memory/Byte.h>

namespace nox
{
    namespace memory
    {
        /**
          * @brief      A memory pool that allocates memory blocks with
          *             slotCount number of slots of slotSize size.
          *
          * @tparam     slotSize   How big shall each slot be.
          * @tparam     slotCount  How many slots do we have per block.
          */ 
        template<std::size_t slotSize, std::size_t slotCount>
        class FixedSlotPool
        {
        public:
            /**
             * @brief      Creates the FixedSlotPool with as many blocks as
             *             specified by the parameter.
             *
             * @param[in]  initialBlockCount  The initial number of blocks. Must be > 0.
             */
            FixedSlotPool(std::size_t initialBlockCount = 1);

            /**
             * @brief      Type is only movable.
             */
            FixedSlotPool(const FixedSlotPool&) = delete;

            /**
             * @brief      Type is only movable.
             */
            FixedSlotPool& operator=(const FixedSlotPool&) = delete;

            /**
             * @brief      Move constructor. Moves all members out of source.
             *
             * @warning    source will not be usable after moving. 
             * 
             * @param[in]  source  the FixedSlotPool to move from.
             */
            FixedSlotPool(FixedSlotPool&& source);

            /**
             * @brief      Move assignment operator. Moves all members out of
             *             source. The class is tolerant of self-assignment.
             *
             * @warning    source will not be usable after moving.
             *
             * @param[in]  source  the FixedSlotPool to move from.
             *
             * @return     *this after assignment.
             */
            FixedSlotPool& operator=(FixedSlotPool&& source);

            /**
             * @brief      Destroys all blocks in the pool. The user must ensure
             *             that none of the slots holds any non destructed
             *             objects.
             */
            ~FixedSlotPool();

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
             */ 
            void reset();

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
                Byte slots[slotSize * slotCount];
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

#include <nox/memory/FixedSlotPool.tpp>

#endif
