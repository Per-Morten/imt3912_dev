#ifndef NOX_ECS_EVENTARGUMENT_H_
#define NOX_ECS_EVENTARGUMENT_H_
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/Event.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Creates an argument and puts it into event.
         *
         * @param      event       The event to add the argument to.
         * @param[in]  argument    The argument value to put into the event.
         * @param[in]  identifier  The name of the argument.
         *
         * @tparam     T           c style arrays are illegal. 
         *                         sizeof(T) + sizeof(Event::Argument) <=
         *                         Event::ArgumentAllocator::MAX_SIZE
         */
        template<class T>
        void
        createEventArgument(Event& event,
                            const T& argument,
                            const TypeIdentifier& identifier);
    }
}

#include <nox/ecs/createEventArgument.tpp>

#endif
