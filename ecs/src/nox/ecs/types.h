#ifndef NOX_ECS_TYPES_H_
#define NOX_ECS_TYPES_H_
#include <cstdint>

namespace nox
{
    namespace ecs
    {
        /**
         *  Used to identify entities across the ECS.
         *  Unique per entity.
         */
        using EntityId = std::size_t;
    }
}

#endif
