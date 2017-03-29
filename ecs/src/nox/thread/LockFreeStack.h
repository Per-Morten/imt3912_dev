#ifndef NOX_THREAD_LOCKFREESTACK_H_
#define NOX_THREAD_LOCKFREESTACK_H_
#include <atomic>

#include <nox/memory/LockFreeAllocator.h>

namespace nox
{
    namespace thread
    {
        // Based on: http://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
        // The push were taken directly from the link, with some changes to the memory order.
        // Those changes might be pessimistic, but we are more certain about their correctness.
        // 
        
        /**
         * @brief      A minimalistic implementation of a lock-free stack. The
         *             stack can be pushed to, popped from, and cleared. The
         *             stack should work for a multiple-producer
         *             multiple-consumer case, however it has only been tested
         *             in a multiple-producer single-consumer case.
         *
         * @tparam     T     Must be of move assignable type, as the pop
         *                   function uses move to into value.
         */
        template<class T>
        class LockFreeStack
        {
        public:
            /**
             * @brief      Creates the stack
             */
            LockFreeStack() = default;

            /**
             * @brief      As a result of the atomics, the type is non-copy-constructible.
             */
            LockFreeStack(const LockFreeStack&) = delete;

            /**
             * @brief      As a result of the atomics, the type is non-copy-assignable.
             */
            LockFreeStack& operator=(const LockFreeStack&) = delete;

            /**
             * @brief      As a result of the atomics, the type is non-move-constructible.
             */
            LockFreeStack(LockFreeStack&&) = delete;

            /**
             * @brief      As a result of the atomics, the type is non-move-assignable.
             */
            LockFreeStack& operator=(LockFreeStack&&) = delete;

            /**
             * @brief      Destroys the object.
             */
            ~LockFreeStack();

            /**
             * @brief      Pushes the value onto the stack.
             *
             * @param[in]  value  The value to push onto the stack.
             */
            void
            push(const T& value);

            /**
             * @brief      Pushes the value onto the stack.
             *
             * @param[in]  value The value to push onto the stack.
             */
            void
            push(T&& value);

            /**
             * @brief      Pops the top value of the stack if possible and
             *             stores it in value.
             *
             * @param[out] value  The value to store the popped value in. If no
             *                    value could be popped from the stack, value is
             *                    left unchanged.
             *
             * @return     True if a value could be popped of the stack, false
             *             otherwise. False then also indicates that at the time
             *             of pop, the stack was empty.
             */
            bool
            pop(T& value);

            /**
             * @brief      Removes and deallocates all the memory used in the
             *             stack at the point in time the function was called.
             *            
             * @warning    Function cannot be called concurrently with push,
             *             as this could corrupt memory with the linear allocator.
             */
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
