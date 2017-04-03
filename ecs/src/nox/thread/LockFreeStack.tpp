#include <nox/thread/Atomic.h>

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

    newHead->next = this->head.load(NOX_ATOMIC_RELAXED);

    while (!head.compare_exchange_weak(newHead->next,
                                       newHead,
                                       NOX_ATOMIC_RELEASE,
                                       NOX_ATOMIC_RELAXED))
    {
    }
}

template<class T>
void
nox::thread::LockFreeStack<T>::push(T&& value)
{
    auto memory = this->allocator.allocate(sizeof(Node));
    auto newHead = new (memory) Node(std::move(value));

    newHead->next = this->head.load(NOX_ATOMIC_RELAXED);

    while (!head.compare_exchange_weak(newHead->next,
                                       newHead,
                                       NOX_ATOMIC_RELEASE,
                                       NOX_ATOMIC_RELAXED))
    {
    }
}

template<class T>
bool
nox::thread::LockFreeStack<T>::pop(T& value)
{
    auto expected = this->head.load(NOX_ATOMIC_ACQUIRE);
    if (!expected)
    {
        return false;
    }

    while (!head.compare_exchange_weak(expected,
                                       expected->next,
                                       NOX_ATOMIC_RELEASE,
                                       NOX_ATOMIC_RELAXED))
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
    auto itr = this->head.exchange(nullptr, NOX_ATOMIC_ACQ_REL);
    while (itr)
    {
        itr->~Node();
        this->allocator.deallocate(itr);
    }

    this->allocator.clear();
}
