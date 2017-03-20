#include <nox/util/nox_assert.h>

template<std::size_t slotSize, std::size_t slotCount>
nox::memory::FixedSlotPool<slotSize, slotCount>::FixedSlotPool(std::size_t initialBlockCount) 
{
    NOX_ASSERT(initialBlockCount > 0, "At least one block must be allocated!");
    first = new Block();
    firstFree = first;

    // Doing -1 because we have already done the first block.
    auto itr = first;
    for (std::size_t i = 0; i < initialBlockCount - 1; ++i)
    {
        itr->next = new Block();
        itr = itr->next;
    }
}

template<std::size_t slotSize, std::size_t slotCount>
nox::memory::FixedSlotPool<slotSize, slotCount>::FixedSlotPool(FixedSlotPool&& source)
    : first(source.first)
    , firstFree(source.firstFree)
{
    source.first = nullptr;
    source.firstFree = nullptr;
}

template<std::size_t slotSize, std::size_t slotCount>
nox::memory::FixedSlotPool<slotSize, slotCount>& 
nox::memory::FixedSlotPool<slotSize, slotCount>::operator=(FixedSlotPool&& source)
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

template<std::size_t slotSize, std::size_t slotCount>
nox::memory::FixedSlotPool<slotSize, slotCount>::~FixedSlotPool()
{
    while (this->first != nullptr)
    {
        auto tmp = this->first->next;
        delete this->first;
        this->first = tmp;   
    }
}

template<std::size_t slotSize, std::size_t slotCount>
void* 
nox::memory::FixedSlotPool<slotSize, slotCount>::allocate(std::size_t size)
{
    NOX_ASSERT(size == slotSize, "Wrong allocate size passed! Template: %zu, Argument: %zu", slotSize, size);
    if (this->firstFree->used >= slotSize * slotCount)
    {
        // Might be that we are reusing a block here, which can happen after we have done a reset.
        auto newBlock = (this->firstFree->next != nullptr) ? this->firstFree->next : new Block();
        this->firstFree->next = newBlock;
        this->firstFree = newBlock;
        this->firstFree->used = 0;
    }
    auto ret = &(this->firstFree->slots[this->firstFree->used]);
    this->firstFree->used += slotSize;
    return ret;
}

template<std::size_t slotSize, std::size_t slotCount>
void 
nox::memory::FixedSlotPool<slotSize, slotCount>::reset()
{
    this->first->used = 0;
    this->firstFree = this->first;
}
