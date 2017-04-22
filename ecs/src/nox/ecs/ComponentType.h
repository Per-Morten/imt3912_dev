#ifndef NOX_ECS_COMPONENTTYPE_H_
#define NOX_ECS_COMPONENTTYPE_H_
#include <cstddef>

namespace nox
{
    namespace ecs 
    {
        namespace component_type
        {
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
        }
    }
}

#endif
