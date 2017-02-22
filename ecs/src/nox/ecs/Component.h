#ifndef NOX_ECS_COMPONENT_H_
#define NOX_ECS_COMPONENT_H_
#include <nox/ecs/EntityId.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief Base class for all components.   
         *        This is the only "type" that the EntityManager can interact with.
         */
        class Component
        {
        public:
            /**
             * @brief Used to identify which entity this component belongs to.
             */
            const EntityId entityId;

            /**
             * @brief Deleted as creating components without id's are illegal.
             */
            Component() = delete;

            /**
             * @brief Creates a component with the given entityId.
             *
             * @param entityId the id of the entity this component is tied to.
             */
            inline Component(const EntityId& entityId);

            /**
             * @brief Copying components is illegal, they are only movable.
             */
            Component(const Component&) = delete;

            /**
             * @brief Copying components is illegal, they are only movable.
             */
            Component& operator=(const Component&) = delete;

            /**
             * @brief Components are only movable.
             */
            Component(Component&&) = default;

            /**
             * @brief Components are only movable.
             */
            Component& operator=(Component&&) = default;
        };
    }
}

#include <nox/ecs/Component.ipp>
#endif
