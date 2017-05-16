#include <nox/util/nox_assert.h>
#include <cstring>

template<std::size_t blockSize>
nox::memory::LinearAllocator<blockSize>::LinearAllocator(std::size_t initialBlockCount) 
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
nox::memory::LinearAllocator<blockSize>::LinearAllocator(LinearAllocator&& source)
    : first(source.first)
    , firstFree(source.firstFree)
{
    source.first = nullptr;
    source.firstFree = nullptr;
}

template<std::size_t blockSize>
nox::memory::LinearAllocator<blockSize>& 
nox::memory::LinearAllocator<blockSize>::operator=(LinearAllocator&& source)
{
    if (this != &source)
    {
        // Ensure that we don't leak.
        while (this->first != nullptr)
        {
            auto tmp = this->first->next;
            delete this->first;
            this->first = tmp;   
        }

        this->first = source.first;
        this->firstFree = source.firstFree;

        source.first = nullptr;
        source.firstFree = nullptr;
    }
    return *this;
}

template<std::size_t blockSize>
nox::memory::LinearAllocator<blockSize>::~LinearAllocator()
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
nox::memory::LinearAllocator<blockSize>::allocate(std::size_t size)
{
    NOX_ASSERT(size <= blockSize, "Requesting to large allocation! MAX_SIZE: %zu, Argument: %zu", blockSize, size);
    if (this->firstFree->used + size >= MAX_SIZE)
    {
        // Might be that we are reusing a block here, which can happen after we have done a reset.
        auto newBlock = (this->firstFree->next != nullptr) ? this->firstFree->next : new Block();
        this->firstFree->next = newBlock;
        this->firstFree = newBlock;
    }
    auto ret = &(this->firstFree->slots[this->firstFree->used]);
    this->firstFree->used += size;
    return ret;
}

template<std::size_t blockSize>
void 
nox::memory::LinearAllocator<blockSize>::clear()
{
    auto itr = this->first;
    while (itr)
    {
        itr->used = 0;
        std::memset(itr->slots, 0, MAX_SIZE);
        itr = itr->next;
    }

    this->firstFree = this->first;
}
