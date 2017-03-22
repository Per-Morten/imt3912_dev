#ifndef NOX_MEMORY_HEAPALLOCATOR_H_
#define NOX_MEMORY_HEAPALLOCATOR_H_
#include <cstdint>
#include <limits>

namespace nox
{
    namespace memory
    {
        /**
         * @brief      Wrapper class for general purpose heap allocation using
         *             malloc and free.
         *             
         *             This class will not release memory on destruction, as it does not know
         *             what sort of classes it is holding, and how to destroy those.
         */
        class HeapAllocator
        {
        public:
            constexpr static std::size_t MAX_SIZE = std::numeric_limits<std::size_t>::max();
            /**
             * @brief      Allocates size amount of uninitialized memory.
             *
             * @param[in]  size  The size to allocate in bytes.
             *                   size must be > 0.
             *
             * @return     ptr to newly allocated memory.
             */
            void* allocate(std::size_t size);

            /**
             * @brief      Deallocates the memory pointed to by ptr. Ptr must be
             *             pointing to an object previously allocated through
             *             this allocator.
             *
             * @param      ptr   Ptr to memory to deallocate.
             */
            void deallocate(void* ptr);

            /**
             * @brief      Function is ignored in this allocator. The function
             *             exists only reason for easing performance testing in
             *             real world case. See:
             *             https://github.com/Per-Morten/imt3912_dev/issues/81
             */
            void clear() {}
        };
    }
}


#endif
