#ifndef NOX_ECS_EVENT_H_
#define NOX_ECS_EVENT_H_
#include <limits>
#include <vector>

#include <nox/ecs/EntityId.h>
#include <nox/ecs/TypeIdentifier.h>

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
             * @brief      Class for holding arguments within the event.
             *             The class is a non owning container for memory,
             *             it is the events job to destroy all of its arguments properly.
             */
            class Argument
            {
            public:

                /**
                 * @brief      Friending to get easy access to the destructor.
                 */
                friend ecs::Event;

                /**
                 * @brief      Doing unsigned char rather than std::uint8_t as a
                 *             byte are not necessarily 8 bits.
                 */
                using Byte = unsigned char;

                /**
                 * @brief      Function pointer to allow for destruction while
                 *             maintaining types info.
                 */
                using Destructor = void(*)(Byte*);

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
                Argument(Byte* payload,
                         const TypeIdentifier& identifier,
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
                 *             argument. The user must cast this over to the correct
                 *             pointer.
                 *
                 * @note       The casting need to continue holding the const.
                 *
                 * @return     Pointer to value held within argument.
                 */
                const void* const 
                value() const;

                /**
                 * @brief      Returns the TypeIdentifier of the argument.
                 *
                 * @return     The TypeIdentifier of the argument.
                 */
                const TypeIdentifier& 
                getType() const;

            private:
                TypeIdentifier type;
                Byte* payload;
                Destructor destructor;
            };

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
             * @param[in]  eventType   The event type.
             * @param[in]  senderId    The sender of the event.
             * @param[in]  receiverId  The receiver of the event.
             */
            Event(const TypeIdentifier& eventType,
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
             * @brief      Defaulting the move constructor, should have the intended effect.
             */
            Event(Event&&) = default;

            /**
             * @brief      Defaulting the move assignment operator, should have the intended effect.
             */
            Event& operator=(Event&&) = default;

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
            addArgument(const TypeIdentifier& identifier,
                        Argument argument);

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

        private:
            struct ArgumentPair
            {
                TypeIdentifier identifier;
                Argument arg;
            };

            EntityId receiverId;
            EntityId senderId;
            TypeIdentifier type;

            std::vector<ArgumentPair> arguments;
        };
    }
}

#endif
