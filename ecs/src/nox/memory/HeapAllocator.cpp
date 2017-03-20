#include <nox/memory/HeapAllocator.h>
#include <cstdlib>

#include <nox/util/nox_assert.h>

void*
nox::memory::HeapAllocator::allocate(std::size_t size)
{
    NOX_ASSERT(size > 0, "Trying to allocate %zu bytes, %zu is not > 0", size, size);
    return std::malloc(size);
}

void
nox::memory::HeapAllocator::deallocate(void* ptr)
{
    std::free(ptr);
}
