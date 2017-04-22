#ifndef NOX_ECS_EVENTTYPE_H_
#define NOX_ECS_EVENTTYPE_H_
#include <cstdint>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Namespace containing numerical values for standard event
         *             TypeIdentifier used within the ecs.
         *
         * @warning    NOX ECS reserves the numerical values [0-999] for
         *             TypeIdentifiers to standard events.
         *             Using these numerical values outside of interaction
         *             with the standard events can lead to undefined behavior.
         */
        namespace event_type
        {
            #ifdef NOX_USE_STRING_TYPE_ID
            constexpr auto TRANSFORM_CHANGE = "TRANSFORM_CHANGE";
            constexpr auto TRANSFORM_CHANGE = "TRANSFORM_CHANGE";

            #else
            /**
             * @brief      Constant numerical value used for identifying the
             *             transform_change type through the TypeIdentifier.
             */
            constexpr std::size_t TRANSFORM_CHANGE = 0;
            #endif
        }

        /**
         * @brief      Namespace containing numerical values for standard event
         *             arguments TypeIdentifier used within the ecs.
         *
         * @warning    NOX ECS reserves the numerical values [0-999] for
         *             TypeIdentifiers to standard event arguments.
         *             Using these numerical values outside of interaction
         *             with the standard components can lead to undefined behavior.
         */
        namespace event_arg_type
        {
            #ifdef NOX_USE_STRING_TYPE_ID
            constexpr auto TRANSFORM_CHANGE_POSITION = "TRANSFORM_CHANGE_POSITION";
            constexpr auto TRANSFORM_CHANGE_ROTATION = "TRANSFORM_CHANGE_ROTATION";
            constexpr auto TRANSFORM_CHANGE_SCALE = "TRANSFORM_CHANGE_SCALE";

            #else
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
