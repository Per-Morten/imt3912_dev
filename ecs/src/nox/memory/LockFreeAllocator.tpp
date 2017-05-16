#include <nox/util/nox_assert.h>
#include <cstring>

#include <nox/thread/Atomic.h>

template<std::size_t blockSize>
nox::memory::LockFreeAllocator<blockSize>::LockFreeAllocator(std::size_t initialBlockCount) 
{
    NOX_ASSERT(initialBlockCount > 0, "At least one block must be allocated!");
    this->first = new Block();
    this->firstFree = this->first;

    // Doing -1 because we have already done the first block.
    auto itr = this->first;
    for (std::size_t i = 0; i < initialBlockCount - 1; ++i)
    {
        itr->next = new Block();
        itr = itr->next;
    }
}

template<std::size_t blockSize>
nox::memory::LockFreeAllocator<blockSize>::~LockFreeAllocator()
{
    while (this->first != nullptr)
    {
        auto tmp = this->first->next;
        delete this->first;
        this->first = tmp;   
    }
}

template<std::size_t blockSize>
void* 
nox::memory::LockFreeAllocator<blockSize>::allocate(const std::size_t size)
{
    NOX_ASSERT(size > 0 && size <= MAX_SIZE, "param size must satisfy 0 < size <= MAX_SIZE, size was: %zu", size);

    Block* desired = nullptr;
    bool allocated = false;

    void* memory = nullptr;

    while (memory == nullptr)
    {
        // Ensure that each time we come around we have as new version of firstFree as possible.
        Block* expected = this->firstFree.load(NOX_ATOMIC_ACQUIRE);
        
        memory = this->tryAllocate(*expected, size);
        if (memory)
        {
            if (allocated)
            {
                delete desired;
            }
            break;
        }

        if (!allocated)
        {
            desired = expected->next;
            if (desired == nullptr)
            {
                desired = new Block();
                allocated = true;
            }
        }

        Block* prev = expected;

        // We need to do an if here, rather than a while. 
        // Because it might be that someone has come before us and set firstFree to "our"
        // desired, and in that case we would get cycles in our list.
        // Doing a weak because spurious wakeup should not hurt us in this case.
        if (this->firstFree.compare_exchange_weak(expected, 
                                                  desired, 
                                                  NOX_ATOMIC_ACQ_REL, 
                                                  NOX_ATOMIC_RELAXED))
        {
            // We know that everyone watching the firstFree variable will now see "desired",
            // meaning it should not be a data race to update prev->next.
            prev->next = desired;
            
            memory = this->tryAllocate(*desired, size);

            // If we have allocated memory, but someone has already used all of it!
            // We will need to do another iteration, but we can't delete desired, 
            // as it is now attached to the list.
            allocated = false;
        }
    }

    return memory;
}

template<std::size_t blockSize>
void 
nox::memory::LockFreeAllocator<blockSize>::clear()
{
    auto itr = this->first;
    while (itr)
    {
        itr->used.store(0, NOX_ATOMIC_RELEASE);
        std::memset(itr->memory, 0, MAX_SIZE);
        itr = itr->next;
    }

    this->firstFree.store(this->first, NOX_ATOMIC_RELEASE);
}

template<std::size_t blockSize>
void* 
nox::memory::LockFreeAllocator<blockSize>::tryAllocate(Block& block, 
                                                       const std::size_t size)
{
    // Only need to load this value once, as it is updated in the compare_exchange.
    std::size_t current = block.used.load(NOX_ATOMIC_ACQUIRE);
    std::size_t desired = 0;
    
    do
    {
        desired = current + size;
        if (desired > MAX_SIZE)
        {
            return nullptr;
        }

    } while (!block.used.compare_exchange_weak(current,
                                               desired,
                                               NOX_ATOMIC_ACQ_REL,
                                               NOX_ATOMIC_RELAXED));

    // At this point we know current < block->used,
    // meaning it should not be a race to send this back.
    return &block.memory[current];
}
