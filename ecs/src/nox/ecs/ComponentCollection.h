#ifndef NOX_ECS_COMPONENTCOLLECTION_H_
#define NOX_ECS_COMPONENTCOLLECTION_H_
#include <cstddef>
#include <memory>
#include <vector>

#include <nox/common/types.h>
#include <nox/ecs/Component.h>
#include <nox/ecs/EntityId.h>
#include <nox/ecs/MetaInformation.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/TypeIdentifier.h>

#include <json/value.h>

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
        class ComponentCollection
        {
        public:
            /**
             * @brief      Default construction of ComponentCollection is
             *             illegal. MetaInformation is needed.
             */
            ComponentCollection() = delete;

            /**
             * @brief      Creates a ComponentCollection for a specified
             *             component type. All information related to the
             *             component type is located in the MetaInformation.
             *
             * @param[in]  info  Info holding all the information about the
             *                   operations to be done on the component.
             */
            ComponentCollection(const MetaInformation& info);

            /**
             * @brief      Copy construction is illegal as component types are
             *             only allowed to be moved.
             */
            ComponentCollection(const ComponentCollection& other) = delete;

            /**
             * @brief      Copy assignment is illegal as component types are
             *             only allowed to be moved.
             */
            ComponentCollection& operator=(const ComponentCollection& other) = delete;

            /**
             * @brief      Move constructor. Moves all members out of source.
             *
             * @warning    source will not be usable after moving. Do not move
             *             after any ComponentHandles have been created with
             *             this collection, it will lead to a dangling pointer!
             *
             * @param[in]  source  the collection to move from.
             */
            ComponentCollection(ComponentCollection&& source);

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
            ComponentCollection& operator=(ComponentCollection&& source);

            /**
             * @brief      Standard destructor, destroys all the objects.
             */
            ~ComponentCollection();

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
            /**
             * @brief      Used within the IndexMap, allowing for faster searches.
             */
            struct ComponentPair
            {
                EntityId id;
                Component* component;
            };

            using IndexMap = std::vector<ComponentPair>;

            /**
             * @brief      Typedef to make it even more explicit that we am
             *             working with bytes. using unsigned char as bytes
             *             rather than std::uint8_t as bytes are not always
             *             guaranteed to be 8 bit.
             */
            using Byte = unsigned char;

            /**
             * @brief      Convenience wrapper for
             *             reinterpret_cast<Component*>(entity)
             *
             * @param      entity  the entity to cast over to a Component*
             *
             * @return     entity reinterpret_casted to a Component*
             */
            Component*
            cast(Byte* entity) const;

            /**
             * @brief      Finds the component whose id matches id.
             *
             * @param[in]  id    the id of the component to look for.
             *
             * @return     iterator with ptr pointing to the component whose id
             *             == id if it can be found. std::end(componentMap)
             *             otherwise.
             *
             * @complexity O(log n)
             */
            IndexMap::iterator
            find(const EntityId& id);

            /**
             * @brief      Finds the component whose id matches id.
             *
             * @param[in]  id    the id of the component to look for.
             *
             * @return     const_iterator with ptr pointing to the component
             *             whose id == id if it can be found.
             *             std::cend(componentMap) otherwise.
             *
             * @complexity O(log n)
             */
            IndexMap::const_iterator
            find(const EntityId& id) const;

            /**
             * @brief      Finds the last iterator whose id < id, Used when we
             *             are inserting into the componentMap, allowing us to
             *             know where to insert.
             *
             * @param[in]  id    The id to compare the components.id to.
             *
             * @return     const_iterator to last component whose id < id,
             *             std::cend if none can be found.
             *
             * @complexity O(log n)
             */
            IndexMap::const_iterator
            findBefore(const EntityId& id) const;

            /**
             * @brief      Calculates the size of the collection in bytes. Used
             *             for internal calculations only, as iterating with
             *             char* requires knowing the value in bytes.
             *
             * @return     size of collection in bytes.
             */
            std::size_t
            size() const;

            /**
             * @brief      Calculates the capacity of the collection in bytes.
             *             Used to figure out when to reallocate.
             *
             * @return     capacity of the collection in bytes.
             */
            std::size_t
            capacity() const;

            /**
             * @brief      Reallocates the collection to another memory area.
             *             this allows for dynamic growth of the container.
             */
            void
            reallocate();

            /**
             * @brief      Destroys all objects in the range [begin, end) calls
             *             destructor on all the objects, but does not do
             *             lifecycle calls as the components are moved when
             *             reallocation happens, and that is the only time this
             *             function is called.
             *
             * @param      begin  The beginning of the range to destroy.
             * @param      end    past-the-end pointer of the range to destroy.
             */
            void
            destroyRange(Byte* begin,
                         Byte* end);

            /**
             * @brief      Swaps the two elements pointed to by lhs and rhs. the
             *             swap area is used.
             *
             * @param      lhs   The value to be swapped.
             * @param      rhs   The value to be swapped.
             */
            void
            swap(Component* lhs,
                 Component* rhs);

            /**
             * @brief      Used to update the componentMap when we have reallocated.
             */
            void
            updateWholeMap();

            /**
             * @brief      Growth factor describing how much the capacity should
             *             grow per reallocation.
             */
            static constexpr std::size_t GROWTH_FACTOR = 2;

            MetaInformation info;
            std::size_t gen{};

            IndexMap componentMap{};

            Byte* active{};
            Byte* inactive{};
            Byte* hibernating{};
            Byte* memory{};
            Byte* cap{};
        };
    }
}

#endif
