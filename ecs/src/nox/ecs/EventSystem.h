#ifndef NOX_ECS_EVENTSYSTEM_H_
#define NOX_ECS_EVENTSYSTEM_H_
#include <cstdint>

#include <nox/ecs/Event.h>
#include <nox/memory/Byte.h>
#include <nox/memory/LinearAllocator.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Class for handling events within the ECS. Functions sort
         *             of like a queue except that no references are invalidated
         *             before a call to reset, which will invalidate all
         *             references and destroy all events in the queue.
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
             * @brief      Creates an event with the given arguments that is
             *             then handed back to the user for further construction
             *             and customization. The Event is tied to the
             *             EventSystem, and will be deallocated when a reset is
             *             called.
             *
             * @param[in]  eventType   The event type.
             * @param[in]  senderId    The sender of the event.
             * @param[in]  receiverId  The receiver of the event.
             *
             * @return     Reference to a new event that can be customized
             *             further.
             */
            Event& createEvent(const TypeIdentifier& eventType,
                               const EntityId& senderId,
                               const EntityId& receiverId = nox::ecs::Event::BROADCAST);

            /**
             * @brief      Gets the next event that needs to be processed in the
             *             event system.
             *
             * @warning    Calling readNextEvent on an empty EventSystem is
             *             undefined behavior.
             *
             * @return     The next event to process.
             */
            Event& readNextEvent();

            /**
             * @brief      Removes and deallocates all memory from the
             *             container.
             */ 
            void clear();

            /**
             * @brief      Checks if the container is empty.
             *
             * @return     True if the container is currently empty, false
             *             otherwise.
             */
            bool empty();
            
        private:
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
                EventNode(Event e)
                    : event(std::move(e))
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
             * @brief Shared among all the different events, used to allocate memory for arguments.
             */
            Event::ArgumentAllocator argumentAllocator{};

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
