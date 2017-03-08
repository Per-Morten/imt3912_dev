#ifndef NOX_ECS_PARENT_H_
#define NOX_ECS_PARENT_H_
#include <nox/ecs/Component.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Component allowing for entities to query for their
         *             parents.
         */
        class Parent
            : public Component
        {
        public:
            using nox::ecs::Component::Component;

            /**
             * @brief      Move constructor for Parent component.
             *
             * @param[in]  source The component to move from.
             */
            Parent(Parent&& other);

            /**
             * @brief      Move assignment operator for Parent component. The
             *             function is tolerant of self assignment.
             *
             * @param[in]  source The component to move from.
             *
             * @return     *this
             */
            Parent& operator=(Parent&& other);

            /**
             * @brief      Id of the parent to the entity owning this component.
             * 
             * @warning    Changing this will not have the intended effect!
             *             The variable is public mainly to allow for move assignment.
             */ 
            EntityId parentId{};
        };
    }
}

#endif
