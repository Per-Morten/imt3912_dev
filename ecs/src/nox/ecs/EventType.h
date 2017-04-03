#ifndef NOX_ECS_EVENTTYPE_H_
#define NOX_ECS_EVENTTYPE_H_
#include <cstdint>

namespace nox
{
    namespace ecs
    {
        namespace event_type
        {
            #ifdef NOX_USE_STRING_TYPE_ID
            #pragma message "using string values for TypeIdentifier"
            constexpr auto TRANSFORM_CHANGE = "TRANSFORM_CHANGE";

            #else
            #pragma message "using numeric values for TypeIdentifier"
            /**
             * @brief      Constant numerical value used for identifying the
             *             transform_change type through the TypeIdentifier.
             */ 
            constexpr std::size_t TRANSFORM_CHANGE = 0;
            #endif
        }

        namespace event_arg_type
        {
            #ifdef NOX_USE_STRING_TYPE_ID
            #pragma message "using string values for TypeIdentifier"
            constexpr auto TRANSFORM_CHANGE_POSITION = "TRANSFORM_CHANGE_POSITION";
            constexpr auto TRANSFORM_CHANGE_ROTATION = "TRANSFORM_CHANGE_ROTATION";
            constexpr auto TRANSFORM_CHANGE_SCALE = "TRANSFORM_CHANGE_SCALE";

            #else
            #pragma message "using numeric values for TypeIdentifier"
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
            #endif
        }
    }
}


#endif
