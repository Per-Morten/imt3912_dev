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

    newHead->next = this->head.load(std::memory_order_acquire);

    while (!head.compare_exchange_weak(newHead->next, 
                                       newHead,
                                       std::memory_order_acq_rel,
                                       std::memory_order_acquire))
    {
    }
}

template<class T>
void
nox::thread::LockFreeStack<T>::push(T&& value)
{
    auto memory = this->allocator.allocate(sizeof(Node));
    auto newHead = new (memory) Node(std::move(value));

    newHead->next = this->head.load(std::memory_order_acquire);

    while (!head.compare_exchange_weak(newHead->next, 
                                       newHead,
                                       std::memory_order_acq_rel,
                                       std::memory_order_acquire))
    {
    }
}

template<class T>
bool
nox::thread::LockFreeStack<T>::pop(T& value)
{
    auto expected = this->head.load(std::memory_order_acquire);
    if (expected)
    {
        while (!head.compare_exchange_weak(expected,
                                           expected->next,
                                           std::memory_order_acq_rel,
                                           std::memory_order_acquire))
        {
        }

        value = std::move(expected->value);
        expected->~Node();
        this->allocator.deallocate(expected);

        return true;
    }

    return false;
}

template<class T>
void
nox::thread::LockFreeStack<T>::clear()
{
    auto itr = this->head.load();
    while (itr)
    {
        itr->~Node();
        this->allocator.deallocate(itr);
    }

    this->allocator.clear();
}
