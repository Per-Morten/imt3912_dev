#ifndef NOX_ECS_CHILDREN_H_
#define NOX_ECS_CHILDREN_H_
#include <vector>
#include <nox/ecs/Component.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Component allowing for entities to have children.
         */
        class Children
            : public Component
        {
        public:
            using nox::ecs::Component::Component;
            
            /**
             * @brief      Move constructor for Children component.
             *
             * @param[in]  source The component to move from.
             */
            Children(Children&& source);

            /**
             * @brief      Move assignment operator for Children component.
             *             The function is tolerant of self assignment.
             *
             * @param[in]  source The children to move from.
             *
             * @return     *this
             */
            Children& operator=(Children&& source);

            /**
             * @brief      Adds a child to the children component.
             *             Duplicates are not added.
             *
             * @param[in]  childId  The id of the child of this entity.
             */
            void addChild(const EntityId& childId);

            /**
             * @brief      Removes the child specified by childId.
             *
             * @param[in]  childId  The id of the child to remove.
             */
            void removeChild(const EntityId& childId);

            /**
             * @brief      Checks if this component has a specific child.
             *
             * @param[in]  childId  The id of the child to check for.
             *
             * @return     True if the entity this component belongs to has a
             *             child with id == childId, false otherwise.
             */
            bool hasChild(const EntityId& childId);
           
            /**
             * @brief      Gets the id of the child located at position index.
             *             No bounds checking is performed.
             *
             * @param[in]  index The index where the child is located.
             *
             * @return     The id of the child located at index == index.
             */
            EntityId operator[](std::size_t index) const;

            /**
             * @brief      Checks the size of the container.
             *
             * @return     The size of the container.
             */
            std::size_t size() const;

            /**
             * @brief      Checks if the children component is empty, i.e. has no children.
             *
             * @return     True if the component have no children, false otherwise.
             */
            bool empty() const;

        private:
            std::vector<EntityId> children{};
        };
    }
}

#endif
