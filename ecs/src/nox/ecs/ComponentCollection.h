#ifndef NOX_ECS_COMPONENTCOLLECTION_H_
#define NOX_ECS_COMPONENTCOLLECTION_H_
#include <memory>
#include <cstdint>

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
         *        in the container.
         *        However it will invalidate iterators from time to time.
         *        Use the ComponentHandle to avoid this.
         */
        class ComponentCollection
        {
        public:
            /**
             * @brief Default construction of ComponentCollection is illegal.
             *        MetaInformation is needed. 
             */ 
            ComponentCollection() = delete;

            /**
             * @brief Creates a ComponentCollection for a specified component type.
             *        All information related to the component type is located in the
             *        MetaInformation.
             *
             * @param[in] info Info holding all the information about the operations
             *                 to be done on the component.
             */
            ComponentCollection(const MetaInformation& info);

            /**
             * @brief Copy construction is illegal as component types are only allowed to be moved.
             */
            ComponentCollection(const ComponentCollection& other) = delete;

            /**
             * @brief Copy assignment is illegal as component types are only allowed to be moved.
             */
            ComponentCollection& operator=(const ComponentCollection& other) = delete;

            /**
             * @brief Move constructor. Moves all members out of source.
             * 
             * @warning source will not be usable after moving.
             *          Do not move after any ComponentHandles have been
             *          created with this collection, it will lead to a dangling pointer!
             *
             * @param[in] source the collection to move from.
             */
            ComponentCollection(ComponentCollection&& source);

            /**
             * @brief Move assignment operator. Moves all members out of source.
             *
             * @warning source will not be usable after moving.
             *          Do not move after any ComponentHandles have been
             *          created with this collection, it will lead to a dangling pointer!
             *
             * @param[in] source the collection to move from.
             * 
             * @return *this after assignment.
             */
            ComponentCollection& operator=(ComponentCollection&& source);

            /**
             * @brief Standard destructor, destroys all the objects.
             */
            ~ComponentCollection();

            /**
             * @brief Creates a component with the specified id.
             *        The component is not initialized, awake or activated,
             *        those steps happens with the help of the other functions.
             *
             * @param[in] id the id of the entity this component belongs to.
             */
            void
            create(const EntityId& id);

            /**
             * @brief Initializes the component with the specified id with
             *        the values from the value parameter.
             *
             * @param[in] id     the id of the entity this component belongs to.
             * @param[in] value  the value to initialize the object with.
             */
            void
            initialize(const EntityId& id, 
                       const Json::Value& value);

            /**
             * @brief Wakes up the component with the given id.
             *        The awake function is called on the component.
             *        if id does not exist, or if MetaInformation.awake == nullptr
             *        nothing happens. 
             *
             * @param[in] id the id of the entity the component belongs to.
             */
            void
            awake(const EntityId& id);

            /**
             * @brief Activates the component with the given id.
             *        The activate function is called on the component.
             *        if id does not exist, or if MetaInformation.activate == nullptr
             *        nothing happens. 
             *
             * @param[in] id the id of the entity the component belongs to.
             */
            void
            activate(const EntityId& id);

            /**
             * @brief Deactivates the component with the given id.
             *        The deactivate function is called on the component.
             *        if id does not exist, or if MetaInformation.deactivate == nullptr
             *        nothing happens. 
             *
             * @param[in] id the id of the entity the component belongs to.
             */
            void
            deactivate(const EntityId& id);
            
            /**
             * @brief Hibernates the component with the given id.
             *        The hibernate function is called on the component.
             *        if id does not exist, or if MetaInformation.hibernate == nullptr
             *        nothing happens. 
             *
             * @param[in] id the id of the entity the component belongs to.
             */
            void
            hibernate(const EntityId& id);

            /**
             * @brief Deletes the component with the given id.
             * 
             * @warning This will destroy the component.
             *
             * @param[in] id the id of the entity that will be removed.
             */
            void
            remove(const EntityId& id);

            /**
             * @brief Calls update on all the active components
             *        within the collection.
             *
             * @param[in] deltaTime the time since this function last was called.
             */
            void
            update(const nox::Duration& deltaTime);

            /**
             * @brief Calls the receiveLogicEvents on all components that shall receive
             *        events based on their lifecycle state. 
             *
             * @param[in]  event  The event to send.
             */
            void
            receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event);
           
            /**
             * @brief Returns the number of components within the collection.
             *
             * @note This is not necessarely (and probably not) the actual size of allocated
             *       memory by the container, as it does a growth based allocation strategy
             *       akin to that of std::vector.
             *
             * @return number of components within the collection.
             */
            std::size_t
            count() const;
 
            /**
             * @brief Returns a ComponentHandle to the component belonging to
             *        the entity identified by id.
             *
             * @param[in] id the id of the entity the component belongs to.
             *
             * @return Handle to the component if found, handle with nullptr otherwise.
             */
            ComponentHandle<Component>
            getComponent(const EntityId& id);

            /**
             * @brief Returns the current generation that this Collection is in.
             *        I.e. How many times has iterators been invalidated. 
             *
             * @return The current generation.
             */
            std::size_t
            getGeneration() const;

            /**
             * @brief Returns the type identifier of the components in this collection.
             *
             * @return The type identifier indicating the type of the components in this collection.
             */
            const TypeIdentifier&
            getTypeIdentifier() const;

        private:
            /**
             * @brief Typedef to make it even more explicit that I am working with bytes.
             *        using unsigned char as bytes rather than std::uint8_t as
             *        bytes are not always guaranteed to be 8 bit.
             */
            using Byte = unsigned char;

            /**
             * @brief Convenience wrapper for reinterpret_cast<Component*>(entity)
             *
             * @param entity the entity to cast over to a Component*
             *
             * @return entity reinterpret_casted to a Component*
             */
            Component*
            cast(Byte* entity) const;

            /**
             * @brief Finds the component whose id matches the id.
             *
             * @param[in] id the id of the component to look for.
             *
             * @return the component where id == id, nullptr if none can be found.
             */
            Component*
            find(const EntityId& id) const;

            /**
             * @brief Calculates the size of the collection in bytes.
             *        Used for internal calculations only, as iterating with
             *        char* requires knowing the value in bytes. 
             *
             * @return size of collection in bytes.
             */
            std::size_t
            size() const;

            /**
             * @brief Calculates the capacity of the collection in bytes.
             *        Used to figure out when to reallocate.
             *
             * @return capacity of the collection in bytes.
             */
            std::size_t
            capacity() const;

            /**
             * @brief Reallocates the collection to another memory area.
             *        this allows for dynamic growth of the container.
             */
            void
            reallocate();

            /**
             * @brief Destroys all objects in the range [begin, end)
             *        calls destructor on all the objects, but does not do lifecycle calls
             *        as the components are moved when reallocation happens, and that is
             *        the only time this function is called. 
             *
             * @param begin  The beginning of the range to destroy.
             * @param end    past-the-end pointer of the range to destroy.
             */
            void
            destroyRange(Byte* begin, Byte* end);

            /**
             * @brief Growth factor describing how much the capacity should grow per reallocation.
             */
            static constexpr std::size_t GROWTH_FACTOR = 2; 

            MetaInformation info;
            std::size_t gen{};
            Byte* first{};
            Byte* last{};
            Byte* cap{};
        };
    }
}

#endif
