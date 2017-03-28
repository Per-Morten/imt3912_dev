#ifndef NOX_THREAD_LOCKFREESTACK_H_
#define NOX_THREAD_LOCKFREESTACK_H_
#include <atomic>
#include <type_traits>

#include <nox/memory/LockFreeAllocator.h>

namespace nox
{
    namespace thread
    {
        // Based on: http://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
        // The push were taken directly from the link, with some changes to the memory order.
        template<class T>
        class LockFreeStack
        {
        public:
            LockFreeStack() = default;
            LockFreeStack(const LockFreeStack&) = delete;
            LockFreeStack& operator=(const LockFreeStack&) = delete;
            LockFreeStack(LockFreeStack&&) = delete;
            LockFreeStack& operator=(LockFreeStack&&) = delete;
            ~LockFreeStack();

            void
            push(const T& value);

            void
            push(T&& value);

            bool
            pop(T& value);

            void
            clear();

        private:
            struct Node 
            {
                Node(T value)
                    : value(std::move(value))
                {
                }

                Node* next{};
                T value;
            };

            using Allocator = nox::memory::LockFreeAllocator<sizeof(Node) * 128>;

            Allocator allocator{};

            std::atomic<Node*> head{};
        };
    }
}

#include <nox/thread/LockFreeStack.tpp>

#endif
