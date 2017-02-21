#ifndef NOX_ECS_ICOMPONENT_H_
#define NOX_ECS_ICOMPONENT_H_
#include <nox/ecs/EntityId.h>

namespace nox
{
    namespace ecs
    {
        /**
         *  Base class for all components.   
         *  This is the only "type" that the EntityManager can interact with.
         */
        struct IComponent
        {
            /**
             *  Used to identify which entity this component belongs to.
             *  
             *  @warning Do not change this! 
             *           It is only non-const to allow for the type 
             *           to stay trivial.
             */
            EntityId entityId;
        };
    }
}

#endif
