#ifndef NOX_ECS_UNIQUEPTRCOMPONENTCOLLECTION_H_
#define NOX_ECS_UNIQUEPTRCOMPONENTCOLLECTION_H_
#include <cstdint>
#include <memory>
#include <vector>
#include <memory>

#include <nox/common/types.h>
#include <nox/ecs/Component.h>
#include <nox/ecs/EntityId.h>
#include <nox/ecs/MetaInformation.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/TypeIdentifier.h>

#include <json/value.h>

#ifdef NOX_ECS_COMPONENT_UNIQUE_PTR_VIRTUAL

namespace nox
{
    namespace ecs
    {
        /**
         * @brief Class created for holding several components of the same type.
         *        It has a pooling mechanism and reuses as much memory as possible.
         *        It will not be fragmented as holes are filled up by other elements
         *        in the container. Because of this the container can be viewed as unordered.
         *        Iterators into the container will be invalidated each time the underlying
         *        structure is changed, or on memory transitions.
         *        Use the ComponentHandle to avoid the problem with iterator invalidation.
         *
         * @detail The components are stored in different areas of memory based on
         *         what part of the lifecycle they are in.
         *         The different pointers of the class points to the different areas of memory,
         *         as shown below:
         *         --------------------------------------------------------------------------------------------------
         *         | active               | inactive             |  hibernating       | raw memory           | swap |
         *         --------------------------------------------------------------------------------------------------
         *         ^active*               ^inactive*             ^hibernating*        ^memory*               ^cap*
         *
         *         An extra area called the swap area is also allocated, this is used when swapping components,
         *         as we can't stack allocate the components when we don't know the size.
         *
         * @see    nox::ecs::ComponentHandle
         */
        class UniquePtrComponentCollection
        {
        public:
            /**
             * @brief      Default construction of UniquePtrComponentCollection is
             *             illegal. MetaInformation is needed.
             */
            UniquePtrComponentCollection() = delete;

            /**
             * @brief      Creates a UniquePtrComponentCollection for a specified
             *             component type. All information related to the
             *             component type is located in the MetaInformation.
             *
             * @param[in]  info  Info holding all the information about the
             *                   operations to be done on the component.
             */
            UniquePtrComponentCollection(const MetaInformation& info);

            /**
             * @brief      Copy construction is illegal as component types are
             *             only allowed to be moved.
             */
            UniquePtrComponentCollection(const UniquePtrComponentCollection& other) = delete;

            /**
             * @brief      Copy assignment is illegal as component types are
             *             only allowed to be moved.
             */
            UniquePtrComponentCollection& operator=(const UniquePtrComponentCollection& other) = delete;

            /**
             * @brief      Move constructor. Moves all members out of source.
             *
             * @warning    source will not be usable after moving. Do not move
             *             after any ComponentHandles have been created with
             *             this collection, it will lead to a dangling pointer!
             *
             * @param[in]  source  the collection to move from.
             */
            UniquePtrComponentCollection(UniquePtrComponentCollection&& source) = default;

            /**
             * @brief      Move assignment operator. Moves all members out of
             *             source. The class is tolerant of self-assignment.
             *
             * @warning    source will not be usable after moving. Do not move
             *             after any ComponentHandles have been created with
             *             this collection, it will lead to a dangling pointer!
             *
             * @param[in]  source  the collection to move from.
             *
             * @return     *this after assignment.
             */
            UniquePtrComponentCollection& operator=(UniquePtrComponentCollection&& source) = default;

            /**
             * @brief      Standard destructor, destroys all the objects.
             */
            ~UniquePtrComponentCollection() = default;

            /**
             * @brief      Creates a component with the specified id. The
             *             component is not initialized, awake or activated,
             *             those steps happens with the help of the other
             *             functions.
             *
             * @param[in]  id             the id of the entity the new component
             *                            belongs to.
             * @param      entityManager  The entityManager controlling the
             *                            components.
             */
            void
            create(const EntityId& id,
                   EntityManager* entityManager);

            /**
             * @brief      Adopts the component pointed to by component
             *             parameter. The component is moved from, and is not
             *             usable afterwards. Choose the name adopt rather than
             *             overloading create
             *
             * @param      component  The component is moved from, and should
             *                        not be considered usable.
             */
            void
            adopt(Component& component);

            /**
             * @brief      Initializes the component with the specified id with
             *             the values from the value parameter.
             *
             * @param[in]  id     the id of the entity this component belongs
             *                    to.
             * @param[in]  value  the value to initialize the object with.
             */
            void
            initialize(const EntityId& id,
                       const Json::Value& value);

            /**
             * @brief      Wakes up the component with the given id. The
             *             component is marked as awake, and awake is called on
             *             the component if the function exists. Nothing happens
             *             if the object is not found.
             *
             * @param[in]  id    the id of the entity the component belongs to.
             */
            void
            awake(const EntityId& id);

            /**
             * @brief      Activates the component with the given id. The
             *             component is marked as active, and activate is called
             *             on the component if the function exists. Nothing
             *             happens if the object is not found.
             *
             * @param[in]  id    the id of the entity the component belongs to.
             */
            void
            activate(const EntityId& id);

            /**
             * @brief      Deactivates the component with the given id. The
             *             component is marked as deactivated, and deactivate is
             *             called on the component if the function exists.
             *             Nothing happen if the object is not found.
             *
             * @param[in]  id    the id of the entity the component belongs to.
             */
            void
            deactivate(const EntityId& id);

            /**
             * @brief      Hibernates the component with the given id. The
             *             component is marked as hibernating, and hibernate is
             *             called on the component if the function exists.
             *             Nothing happens if the object is not found.
             *
             * @param[in]  id    the id of the entity the component belongs to.
             */
            void
            hibernate(const EntityId& id);

            /**
             * @brief      Deletes the component with the given id.
             *
             * @warning    This will destroy the component.
             *
             * @param[in]  id    the id of the entity that will be removed.
             */
            void
            remove(const EntityId& id);

            /**
             * @brief      Calls update on all the active components within the
             *             collection.
             *
             * @param[in]  deltaTime  the time since this function last was
             *                        called.
             */
            void
            update(const nox::Duration& deltaTime);

            /**
             * @brief      Calls the receiveLogicEvents on all components that
             *             shall receive events based on their lifecycle state.
             *
             * @param[in]  event  The event to send.
             */
            void
            receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event);

            /**
             * @brief      Calls the receiveEntityEvent on all components that
             *             shall receive events based on their lifecycle state.
             *             If the event is supposed to be broadcast all
             *             components will receive the event, otherwise the
             *             collection will do a find to find the correct
             *             component.
             *
             * @param[in]  event  The event to receive.
             */
            void
            receiveEntityEvent(const ecs::Event& event);

            /**
             * @brief      Returns the number of components within the
             *             collection.
             *
             * @note       This is not necessarily (and probably not) the actual
             *             size of allocated memory by the container, as it does
             *             a growth based allocation strategy akin to that of
             *             std::vector.
             *
             * @return     number of components within the collection.
             */
            std::size_t
            count() const;

            /**
             * @brief      Returns a ComponentHandle to the component belonging
             *             to the entity identified by id.
             *
             * @param[in]  id    the id of the entity the component belongs to.
             *
             * @return     Handle to the component if found, handle with nullptr
             *             otherwise.
             */
            ComponentHandle<Component>
            getComponent(const EntityId& id);

            /**
             * @brief      Returns the current generation that this Collection
             *             is in. I.e. How many times has iterators been
             *             invalidated.
             *
             * @return     The current generation.
             */
            std::size_t
            getGeneration() const;

            /**
             * @brief      Returns the type identifier of the components in this
             *             collection.
             *
             * @return     The type identifier indicating the type of the
             *             components in this collection.
             */
            const TypeIdentifier&
            getTypeIdentifier() const;

            /**
             * @brief      Returns the meta information related to this collection.
             *
             * @return     The meta information related to this collection.
             */
            const MetaInformation&
            getMetaInformation() const;

        private:
            using Components = std::vector<std::unique_ptr<Component>>;

            Components::iterator
            find(const EntityId& id);

            Components::const_iterator
            find(const EntityId& id) const;

            Components::iterator
            findBefore(const EntityId& id);

            Components::const_iterator
            findBefore(const EntityId& id) const;

            /**
             * @brief      Growth factor describing how much the capacity should
             *             grow per reallocation.
             */
            static constexpr std::size_t GROWTH_FACTOR = 2;

            MetaInformation info;
            std::size_t gen{0};
            std::vector<std::unique_ptr<Component>> components;
        };
    }
}

#endif
#endif