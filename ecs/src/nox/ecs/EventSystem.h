#ifndef NOX_ECS_EVENTSYSTEM_H_
#define NOX_ECS_EVENTSYSTEM_H_
#include <cstdint>
#include <mutex>

#include <nox/ecs/Event.h>
#include <nox/memory/Byte.h>
#include <nox/memory/LinearAllocator.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Class for handling events within the ECS. Functions sort
         *             of like a queue/stack except that no references are invalidated
         *             before a call to reset, which will invalidate all
         *             references and destroy all events in the queue/stack.
         */
        class EventSystem
        {
        public:
            /**
             * @brief      Constructs the EventSystem with a single head object.
             */
            EventSystem();

            /**
             * @brief      Destroys the objects and deallocates all memory that
             *             has been used for pooling events.
             */
            ~EventSystem();

            /**
             * @brief      Pushes an event onto the EventSystem.
             *
             * @param[in]  event  The event to add to the EventSystem.
             */
            void
            push(Event&& event);

            /**
             * @brief      Pops an event from the System and stores it in event
             *             if possible.
             *
             * @param      event  The event to store the popped value into. If
             *                    no value is popped, the event remains
             *                    unchanged.
             *
             * @return     True if a value is popped and stored in event, false
             *             otherwise.
             */
            bool
            pop(Event& event);

            /**
             * @brief      Removes all elements within the container.
             */ 
            void clear();

        private:
            /**
             * @brief      Checks if the container is empty.
             *
             * @return     True if the container is currently empty, false
             *             otherwise.
             */
            bool empty();

            /**
             * @brief      Base struct used just so the head of the list does
             *             not allocate an extra slot for event. This also
             *             allows for the EventSystem to have a head, even if
             *             Event's are not default constructible.
             */ 
            struct Base
            {
                Base* next{};
            };

            /**
             * @brief      Node holding a pointer to the next Node in the list
             *             as well as holding on to an event.
             */
            struct EventNode : public Base
            {
                EventNode(Event event)
                    : event(std::move(event))
                {
                }
                
                Event event;
            };

            /**
             * @brief      Allocator to be used for allocating EventNodes.
             */
            using NodeAllocator = nox::memory::LinearAllocator<sizeof(EventNode) * 1024>;

            /**
             * @brief      Casts a Base* to an EventNode*
             *
             * @param      ptr   The pointer to cast.
             *
             * @return     ptr casted to EventNode*.
             */
            EventNode* cast(Base* ptr);

            /**
             * @brief       Used to protect all access to this container.
             */ 
            std::mutex mutex{};

            /**
             * @brief      Allocator used for allocating EventNodes.
             */ 
            NodeAllocator nodeAllocator{};

            /**
             * @brief      Head of the list, with pointer to the first element.
             */
            Base head{};

            /**
             * @brief      Ptr to the node before the node to read next.
             */ 
            EventNode* readHead{};

            /**
             * @brief      Ptr to the node before the node to write to next.
             */
            EventNode* writeHead{};
        };
    }
}

#endif
