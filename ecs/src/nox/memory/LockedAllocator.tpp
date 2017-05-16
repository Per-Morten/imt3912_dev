template<class Allocator>
void* 
nox::memory::LockedAllocator<Allocator>::allocate(std::size_t size)
{
    std::lock_guard<std::mutex> lock(mutex);
    return allocator.allocate(size);
}

template<class Allocator>
void
nox::memory::LockedAllocator<Allocator>::deallocate(void* ptr)
{
    std::lock_guard<std::mutex> lock(mutex);
    allocator.deallocate(ptr);
}

template<class Allocator>
void 
nox::memory::LockedAllocator<Allocator>::clear()
{
    std::lock_guard<std::mutex> lock(mutex);
    allocator.clear();
}
