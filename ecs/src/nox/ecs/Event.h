#ifndef NOX_ECS_EVENT_H_
#define NOX_ECS_EVENT_H_
#include <limits>
#include <type_traits>

#include <nox/ecs/EntityId.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/memory/LockFreeAllocator.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Class used for handling communication between different
         *             entities within the ecs.
         */
        class Event
        {
        public:
            /**
             * @brief      Class for holding arguments within the event. The
             *             class is a non owning container for memory, it is the
             *             events job to destroy all of its arguments properly.
             */
            class Argument
            {
            public:
                /**
                 * @brief      Friending event so it gets access to destructor,
                 *             without giving anyone else that access.
                 */
                friend ecs::Event;

                /**
                 * @brief      Function pointer to allow for destruction while
                 *             maintaining types info.
                 */
                using Destructor = void(*)(nox::memory::Byte*);

                /**
                 * @brief      Creating arguments without payload etc is illegal.
                 */
                Argument() = delete;

                /**
                 * @brief      Creates an argument with the given payload,
                 *             identifier and destructor.
                 *
                 * @param      payload     The payload of the argument.
                 * @param[in]  identifier  The type identifier of the type to put in
                 *                         the argument.
                 * @param[in]  destructor  The destructor to use when destroying the
                 *                         argument.
                 */
                Argument(const TypeIdentifier& identifier,
                         nox::memory::Byte* payload,
                         Destructor destructor);

                /**
                 * @brief      Not allowing for copy construction, only movement.
                 */
                Argument(const Argument&) = delete;

                /**
                 * @brief      Now allowing for copy assignment, only movement.
                 */
                Argument& operator=(const Argument&) = delete;

                /**
                 * @brief      Move constructor of the argument.
                 * 
                 * @warning    source is not valid after movement.
                 *
                 * @param[in]  source The argument to move from.
                 */
                Argument(Argument&& source);

                /**
                 * @brief      Move assignment operator. Move assigns from source.
                 *             Class is tolerant of self-assignment.
                 *             
                 * @warning    source is not valid after movement.
                 *             
                 * @param[in]  source The argument to move assign from.
                 *
                 * @return     *this after assignment.
                 */
                Argument& operator=(Argument&& source);

                /**
                 * @brief      Returns a pointer to the value held within the
                 *             argument. The value is interpreted as whatever
                 *             template parameter is given. This version does not
                 *             deep copy the argument, but rather gives a pointer
                 *             into the arguments value. interpreted as the template
                 *             parameter. The user is responsible for ensuring that
                 *             T is of correct type for this argument.
                 *
                 * @tparam     Type  must be a pointer to a const object. The
                 *                   importance is that the type is const, not the
                 *                   pointer.
                 *
                 *             Example: argument.value<const int*>(); // ok
                 *                      argument.value<int* const>(); // not ok
                 *
                 *
                 * @return     Pointer to value held within argument interpreted as
                 *             T type.
                 */
                template<class T = const void*>
                typename std::enable_if<std::is_pointer<T>::value &&
                                        std::is_const<std::remove_pointer_t<T>>::value,
                                        T>::type
                value() const;

                /**
                 * @brief      Returns a copy of the value held within value
                 *             interpreted as type T. The user is responsible for
                 *             ensuring that T is of correct type for this argument.
                 *
                 * @tparam     T     Must be non-pointer and non-reference for a
                 *                   copyable type.
                 *
                 * @return     Copy of the value held within this argument, interpreted as type T.
                 */
                template<class T>
                typename std::enable_if<!std::is_reference<T>::value &&
                                        !std::is_pointer<T>::value,
                                        T>::type
                value() const;

                /**
                 * @brief      Returns the TypeIdentifier of the argument.
                 *
                 * @return     The TypeIdentifier of the argument.
                 */
                const TypeIdentifier& 
                getIdentifier() const;

            private:
                Argument* next{};
                TypeIdentifier identifier;
                nox::memory::Byte* payload;
                Destructor destructor;
            };

            /**
             * @brief      Allocator used for allocating events.
             */
            using ArgumentAllocator = nox::memory::LockFreeAllocator<1024>;
           
            /**
             * @brief      Constant value used to signal that an event shall be
             *             broadcast, not just sent to one entity.
             */
            static const EntityId BROADCAST;

            /**
             * @brief      Creating events within sender, receiver or type is
             *             illegal.
             */
            Event() = delete; 

            /**
             * @brief      Constructs an event that is ready to be used and can
             *             be given arguments.
             *
             * @param      allocator   The allocator that this event will used
             *                         for allocating arguments.
             * @param[in]  eventType   The event type.
             * @param[in]  senderId    The sender of the event.
             * @param[in]  receiverId  The receiver of the event.
             */
            Event(ArgumentAllocator* allocator,
                  const TypeIdentifier& eventType,
                  const EntityId& senderId,
                  const EntityId& receiverId = nox::ecs::Event::BROADCAST);

            /**
             * @brief      Not allowing for copy construction, only movement.
             */
            Event(const Event&) = delete;

            /**
             * @brief      Not allowing for copy assignment, only movement.
             */
            Event& operator=(const Event&) = delete;

             /**
              * @brief      Move constructor. Moves all members out of source.
              *
              * @warning    source will not be usable after moving.
              *
              * @param[in]  source  the Event to move from.
              */
            Event(Event&& source);

            /**
             * @brief      Deleting move assignment operator as references to
             *             allocators prohibits moving properly.
             */
            Event& operator=(Event&& source);

            /**
             * @brief      Destructor, destroys all the arguments belonging to
             *             this event.
             */
            ~Event();

            /**
             * @brief      Adds an argument to the event.
             *
             * @param[in]  identifier  The identifier of the argument.
             * @param[in]  argument    The argument itself. It is moved from and
             *                         is not valid after movement.
             */
            void 
            addArgument(Argument* argument);

            /**
             * @brief      Checks if the event has an argument with the
             *             identifier specified by identifier.
             *
             * @param[in]  identifier  The identifier of the argument to query
             *                         for.
             *
             * @return     True if the event has an argument with identifier ==
             *             identifier. False otherwise.
             */
            bool
            hasArgument(const TypeIdentifier& identifier) const;

            /**
             * @brief      Gets the argument identified by identifier.
             *
             * @warning    No bounds checking is done, looking up an identifier
             *             that does not exist is undefined behavior.
             *
             * @param[in]  identifier  The identifier of the argument to look
             *                         for.
             *
             * @return     The argument whose identifier == identifier.
             */
            const Argument&
            getArgument(const TypeIdentifier& identifier) const;

            /**
             * @brief      Returns the event type.
             *
             * @return     The event type.
             */
            const TypeIdentifier& 
            getType() const;

            /**
             * @brief      Returns the sender of this event.
             *
             * @return     The sender of the event.
             */
            const EntityId&
            getSender() const;

            /**
             * @brief      Returns the receiver of this event.
             *
             * @return     The receiver of the event.
             */
            const EntityId&
            getReceiver() const;

            /**
             * @brief      Returns the allocator belonging to this event.
             *
             * @return     The argumentAllocator belonging to this event.
             *             Mainly used within the createEventArgument function.
             */
            ArgumentAllocator&
            getAllocator();

        private:
            bool isDuplicate(const TypeIdentifier& identifier) const;

            EntityId receiverId;
            EntityId senderId;
            TypeIdentifier type;
            ArgumentAllocator* allocator{nullptr};
            Argument* first{nullptr};
        };
    }
}

#include <nox/ecs/Event.tpp>
#endif
