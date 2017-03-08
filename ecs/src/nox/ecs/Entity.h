#ifndef NOX_ECS_ENTITY_H_
#define NOX_ECS_ENTITY_H_
#include <nox/ecs/EntityId.h>
#include <nox/ecs/EntityManager.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/TypeIdentifier.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Wrapper class for EntityIDs, allowing for manipulation of entities
         *             through an entity class, rather than the EntityManager and id directly.
         *             
         * @detail     Interface mimics that of EntityManager, read there for further explanation.
         * 
         * @see        nox::ecs::EntityManager
         */ 
        class Entity
        {
        public:
            /**
             * @brief Deleted as Entities without id's or
             *        ties to EntityManagers are illegal.
             */ 
            Entity() = delete;

            /**
             * @brief      Sets up the Entity with the given id
             *             and pointer to EntityManager. 
             *
             * @param[in]  id             The id of the entity.
             * @param[in]  entityManager  The entity manager that the entity belongs to.
             *                            Cannot be nullptr!
             */
            Entity(const EntityId& id,
                   EntityManager* entityManager);

            /**
             * @brief      Calls awake on all the entity's components.
             */
            void
            awake();

            /**
             * @brief      Calls activate on all the entity's components.
             */
            void
            activate();

            /**
             * @brief      Calls deactivate on all the entity's components.
             */
            void
            deactivate();

            /**
             * @brief      Calls hibernate on all the entity's components.
             */
            void
            hibernate();

            /**
             * @brief      Removes the entity, deleting all of its components.
             */
            void
            remove();

            /**
             * @brief      Calls awake on the component belonging to this
             *             entity, identified by the identifier.
             *
             * @param[in]  identifier  The type identifier of the component to call
             *                         awake on
             */
            void
            awakeComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Calls activate on the component belonging to this
             *             entity, identified by the identifier.
             *
             * @param[in]  identifier  The type identifier of the component to call
             *                         activate on
             */
            void
            activateComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Calls deactivate on the component belonging to this
             *             entity, identified by the identifier.
             *
             * @param[in]  identifier  The type identifier of the component to call
             *                         deactivate on
             */
            void
            deactivateComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Calls hibernate on the component belonging to this
             *             entity, identified by the identifier.
             *
             * @param[in]  identifier  The type identifier of the component to call
             *                         hibernate on
             */
            void
            hibernateComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Creates a component belonging to this entity,
             *             of the type mapped to the type identifier.
             *
             * @param[in]  identifier  The type identifier of the component type to create.
             */
            void
            assignComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Removes the component belonging to this entity identified by
             *             the type identified.
             *
             * @param[in]  identifier  The type identifier of the component to remove.
             */
            void
            removeComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Returns the component of the type mapped to the identifier,
             *             belonging to this entity.
             *
             * @param[in]  identifier  The type identifier of the component to get.
             *
             * @return     A ComponentHandle to the component if found, handle with nullptr otherwise.
             */
            ComponentHandle<Component>
            getComponent(const TypeIdentifier& identifier);

            /**
             * @brief      Gets the EntityId of this entity.
             *
             * @return     The EntityId of this entity.
             */
            EntityId
            getId() const;

        private:
            EntityId id;
            EntityManager* entityManager;
        };
    }
}

#endif
