#ifndef NOX_ECS_COMPONENTTYPE_H_
#define NOX_ECS_COMPONENTTYPE_H_
#include <cstddef>
#include <string>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Namespace containing numerical values for standard
         *             component TypeIdentifier used within the ecs.
         *
         * @warning    NOX ECS reserves the numerical values [0-999] for
         *             TypeIdentifiers to standard components.
         *             Using these numerical values outside of interaction
         *             with the standard components can lead to undefined behavior.
         */
        namespace component_type
        {
            #ifdef NOX_USE_STRING_TYPE_ID
            #pragma message "Using NOX_USE_STRING_TYPE_ID"

            extern const std::string CHILDREN;
            extern const std::string PARENT;
            extern const std::string TRANSFORM;
            extern const std::string SPRITE;

            #else
            #pragma message "NOT USING NOX_USE_STRING_TYPE_ID"
            /**
             * @brief      Constant numerical value used for identifying the
             *             children type through the TypeIdentifier.
             */
            constexpr std::size_t CHILDREN = 0;

            /**
             * @brief      Constant numerical value used for identifying the
             *             parent type through the TypeIdentifier.
             */
            constexpr std::size_t PARENT = 1;

            /**
             * @brief      Constant numerical value used for identifying the
             *             transform type through the TypeIdentifier.
             */
            constexpr std::size_t TRANSFORM = 2;
            /**
             * @brief      Constant numerical value used for identifying the
             *             sprite type through the TypeIdentifier.
             */
            constexpr std::size_t SPRITE = 3;
            #endif
        }
    }
}

#endif
