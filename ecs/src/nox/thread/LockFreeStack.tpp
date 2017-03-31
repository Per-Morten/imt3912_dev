template<class T>
nox::thread::LockFreeStack<T>::~LockFreeStack()
{
    this->clear();
}

template<class T>
void
nox::thread::LockFreeStack<T>::push(const T& value)
{
    auto memory = this->allocator.allocate(sizeof(Node));
    auto newHead = new (memory) Node(value);

    newHead->next = this->head.load(std::memory_order_relaxed);

    while (!head.compare_exchange_weak(newHead->next,
                                       newHead,
                                       std::memory_order_release,
                                       std::memory_order_relaxed))
    {
    }
}

template<class T>
void
nox::thread::LockFreeStack<T>::push(T&& value)
{
    auto memory = this->allocator.allocate(sizeof(Node));
    auto newHead = new (memory) Node(std::move(value));

    newHead->next = this->head.load(std::memory_order_relaxed);

    while (!head.compare_exchange_weak(newHead->next,
                                       newHead,
                                       std::memory_order_release,
                                       std::memory_order_relaxed))
    {
    }
}

template<class T>
bool
nox::thread::LockFreeStack<T>::pop(T& value)
{
    auto expected = this->head.load(std::memory_order_acquire);
    if (!expected)
    {
        return false;
    }

    while (!head.compare_exchange_weak(expected,
                                       expected->next,
                                       std::memory_order_release,
                                       std::memory_order_relaxed))
    {
    }

    value = std::move(expected->value);
    expected->~Node();
    this->allocator.deallocate(expected);

    return true;
}

template<class T>
void
nox::thread::LockFreeStack<T>::clear()
{
    auto itr = this->head.exchange(nullptr, std::memory_order_acq_rel);
    while (itr)
    {
        itr->~Node();
        this->allocator.deallocate(itr);
    }

    this->allocator.clear();
}
