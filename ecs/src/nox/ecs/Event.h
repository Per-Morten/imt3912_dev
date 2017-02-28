#ifndef NOX_ECS_EVENT_H_
#define NOX_ECS_EVENT_H_
#include <nox/ecs/EntityId.h>

namespace nox
{
    namespace ecs
    {
        struct Event
        {
            virtual ~Event() = default;
        };
    }
}

#endif
