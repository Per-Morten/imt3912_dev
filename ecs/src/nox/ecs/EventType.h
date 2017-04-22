#ifndef NOX_ECS_EVENTTYPE_H_
#define NOX_ECS_EVENTTYPE_H_
#include <cstddef>

namespace nox
{
    namespace ecs
    {
        namespace event_type
        {
            /**
             * @brief      Constant numerical value used for identifying the
             *             transform_change type through the TypeIdentifier.
             */ 
            constexpr std::size_t TRANSFORM_CHANGE = 0;
        }

        namespace event_arg_type
        {
            /**
             * @brief      Constant numerical value used for identifying the 
             *             transform_change_position type through the TypeIdentifier.
             */
            constexpr std::size_t TRANSFORM_CHANGE_POSITION = 0;
            
            /**
             * @brief      Constant numerical value used for identifying the 
             *             transform_change_rotation type through the TypeIdentifier.
             */
            constexpr std::size_t TRANSFORM_CHANGE_ROTATION = 1;

            /**
             * @brief      Constant numerical value used for identifying the 
             *             transform_change_scale type through the TypeIdentifier.
             */
            constexpr std::size_t TRANSFORM_CHANGE_SCALE = 2;
        }
    }
}


#endif
