#ifndef NOX_ECS_EVENTARGUMENT_H_
#define NOX_ECS_EVENTARGUMENT_H_
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/Event.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Creates an argument that can be put into an event.
         *
         * @param[in]  argument    The argument value to put into the event.
         * @param[in]  identifier  The type identifier of the argument.
         *
         * @tparam     T           c style arrays are illegal.
         *
         * @return     An argument that can be put into an event.
         */
        template<class T>
        Event::Argument
        createEventArgument(const T& argument,
                            const TypeIdentifier& identifier);
    }
}

#include <nox/ecs/createEventArgument.tpp>

#endif
