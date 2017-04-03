#ifndef NOX_ECS_ENTITYMANAGER_H_
#define NOX_ECS_ENTITYMANAGER_H_
#include <array>
#include <atomic>
#include <deque>
#include <queue>
#include <vector>

#include <nox/ecs/component/Children.h>
#include <nox/ecs/component/Parent.h>
#include <nox/ecs/ComponentCollection.h>
#include <nox/ecs/UniquePtrComponentCollection.h>

#include <nox/ecs/EntityId.h>
#include <nox/ecs/Event.h>
#include <nox/ecs/Factory.h>
#include <nox/ecs/MetaInformation.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/event/IListener.h>
#include <nox/thread/LockedQueue.h>
#include <nox/thread/LockFreeStack.h>
#include <nox/thread/Pool.h>
#include <nox/util/nox_assert.h>

#include <json/json.h>

namespace nox
{
    namespace ecs
    {
        /**
         * @brief      Class is the central hub for all interaction between the
         *             different entities. Every entity is owned by an
         *             EntityManager, and get their various functions called
         *             through the EntityManager.
         *
         * @detailed   The Interactions with the manager is thread-safe both
         *             from the perspective of the components,
         *             and from the thread who is in charge of the EntityManager
         *             (i.e. The main thread in a single threaded program).
         *
         *             The EntityManager by default runs in a single threaded
         *             model, however it can be turned on to run with a layered
         *             execution model using multiple threads by enabling some
         *             macros. There are some reasons for leaving these macros
         *             off, as the overhead of working with the threads might
         *             surpass the time it takes to execute a function. An
         *             example could be the distributeEntityEvents function, if
         *             your entity events usually are sent directly to a target,
         *             rather than broadcast to all entities, you should
         *             probably not do layered execution on that function.
         *
         *             Update should in general be on, the exception is if there
         *             is no way to parallelize those functions, i.e. all
         *             components have updateAccess == DataAccess::UNKNOWN, or
         *             the dependencies makes them impossible to parallelize.
         *
         *             The macros to use are:
         *
         *             NOX_ECS_LAYERED_EXECUTION_UPDATE
         *             Defining this macro will turn on layered execution
         *             for the updateStep function.
         *
         *             NOX_ECS_LAYERED_EXECUTION_ENTITY_EVENTS
         *             Defining this macro will turn on layered execution
         *             for the distributeEntityEvents function.
         *
         *             NOX_ECS_LAYERED_EXECUTION_LOGIC_EVENTS
         *             Defining this macro will turn on layered execution
         *             for the distributeLogicEvents function.
         */
        class EntityManager final
            : public nox::event::IListener
        {
        public:
            EntityManager() = default;
            EntityManager(const EntityManager&) = delete;
            EntityManager& operator=(const EntityManager&) = delete;
            EntityManager(EntityManager&&) = delete;
            EntityManager& operator=(EntityManager&&) = delete;
            virtual ~EntityManager() override final;

            /**
             * @brief      Creates entity definitions based on the root value
             *             sent in.
             *
             * @todo       Find a way to decouple this so that the EntityManager does
             *             not have to forward everything to the factory.
             *
             * @param[in]  root  The root json value holding all the other json
             *                   values.
             */
            void
            createEntityDefinition(const Json::Value& root);

            /**
             * @brief      Informs the EntityManager that a certain component
             *             type exists. All components types that one wants to
             *             use must be registered.
             *
             * @param[in]  info  The MetaInformation of the component type.
             */
            void
            registerComponent(const MetaInformation& info);

            /**
             * @brief      Configures the EntityManager, settings up the needed
             *             dependencies and sorting the different component
             *             types.
             */
            void
            configureComponents();

            /**
             * @brief      Creates a new EntityId, which is used to identify
             *             entities and components.
             *
             * @return     A unique EntityID which is used when talking about
             *             entities and components.
             */
            EntityId
            createEntity();

            /**
             * @brief      Creates a new EntityId, and assigns the components
             *             needed based on the json value.
             *
             * @note       Component creation is an async operation, the actual
             *             creation of the components will not happen until the
             *             createStep function is run
             *
             * @param[in]  definitionName  The name of the definition to create
             *                             the entity from.
             *
             * @return     A unique EntityID which is used when talking about
             *             entities and components.
             */
            EntityId
            createEntity(const std::string& definitionName);

            /**
             * @brief      Creates and assigns a component to the entity
             *             identified with the id.
             *
             * @note       Creation is an async operation, the actual creation
             *             of the component will not happen until the createStep
             *             function is run.
             *
             * @param[in]  id          The id of the entity to assign the
             *                         component to.
             * @param[in]  identifier  The type identifier of the component
             *                         type.
             */
            void
            assignComponent(const EntityId& id,
                            const TypeIdentifier& identifier);

            /**
              * @brief      Creates and assigns a component to the entity
              *             identified with the id. The component is initialized
              *             with the json value.
              *
              * @note       Creation and initialization are async operations,
              *             the actual creation and initialization of the
              *             component will not happen until the createStep
              *             function is run.
              *
              * @param[in]  id          The id of the entity to assign the
              *                         component to.
              * @param[in]  identifier  The type identifier of the component
              *                         type.
              * @param[in]  value       json value containing the value to
              *                         initialize the component with.
              */ 
            void
            assignComponent(const EntityId& id,
                            const TypeIdentifier& identifier,
                            const Json::Value& value);

            /**
             * @brief      Assigns the children component to the entity
             *             identified with the id. The component is moved from
             *             and should be seen as invalid after this operation.
             *
             * @param[in]  id          The id of the entity to assign the
             *                         component to.
             * @param[in]  identifier  The type identifier of the component
             *                         type. The identifier should be
             *                         component_types::CHILDREN. It is only
             *                         kept around to avoid ambiguity.
             * @param[in]  children    The component to assign to the entity.
             */
            void
            assignComponent(const EntityId& id,
                            const TypeIdentifier& identifier,
                            Children&& children);

            /**
             * @brief      Assigns the parent component to the entity identified
             *             with the id. The component is moved from and should
             *             be seen as invalid after this operation.
             *
             * @param[in]  id          The id of the entity to assign the
             *                         component to.
             * @param[in]  identifier  The type identifier of the component
             *                         type. The identifier should be
             *                         component_types::PARENT. it is only kept
             *                         around to avoid ambiguity.
             * @param[in]  parent      The component to assign to the entity.
             */
            void
            assignComponent(const EntityId& id,
                            const TypeIdentifier& identifier,
                            Parent&& parent);

            /**
             * @brief      Gets the component belonging to the entity with the
             *             given id.
             *
             * @param[in]  id          The id of the entity the component
             *                         belongs to.
             * @param[in]  identifier  The type identifier of the component.
             *
             * @return     A ComponentHandle pointing to the component, or
             *             pointing to nullptr if no component is found.
             */
            ComponentHandle<Component>
            getComponent(const EntityId& id,
                         const TypeIdentifier& identifier);

            /**
             * @brief      Queues up the removal of the component belonging to
             *             the entity with id == id and with 
             *             TypeIdentifier == identifier. 
             *             The component is destroyed, and cannot be recovered.
             *
             * @note       Remove is an async operation, happening in the remove
             *             step.
             *
             * @param[in]  id          The id of the entity the component
             *                         belongs to.
             * @param[in]  identifier  The type identifier of the component.
             */
            void
            removeComponent(const EntityId& id,
                            const TypeIdentifier& identifier);

            /**
             * @brief      Queues up the awakening of the component belonging to
             *             the entity with id == id and 
             *             TypeIdentifier == identifier. 
             *             The component gets its awake function called.
             *
             * @note       Awake is an async operation, happening in the awake
             *             step.
             *
             * @param[in]  id          The id of the entity the component
             *                         belongs to.
             * @param[in]  identifier  The type identifier of the component.
             */
            void
            awakeComponent(const EntityId& id,
                           const TypeIdentifier& identifier);

            /**
             * @brief      Queues up the activation of the component belonging
             *             to the entity with id == id and
             *             TypeIdentifier == identifier.
             *             The component gets its activate function called.
             *
             * @note       Activate is an async operation, happening in the
             *             activate step.
             *
             * @param[in]  id          The id of the entity the component
             *                         belongs to.
             * @param[in]  identifier  The type identifier of the component.
             */
            void
            activateComponent(const EntityId& id,
                              const TypeIdentifier& identifier);

            /**
             * @brief      Queues up the deactivation of the component belonging
             *             to the entity with id == id and
             *             TypeIdentifier == identifier. 
             *             The component gets its deactivate function called.
             *
             * @note       Deactivate is an async operation, happening in the
             *             deactivate step.
             *
             * @param[in]  id          The id of the entity the component
             *                         belongs to.
             * @param[in]  identifier  The type identifier of the component.
             */
            void
            deactivateComponent(const EntityId& id,
                                const TypeIdentifier& identifier);

            /**
             * @brief      Queues up the hibernation of the component belonging
             *             to the entity with id == id and
             *             TypeIdentifier == identifier. 
             *             The component gets its hibernate function called.
             *
             * @note       Hibernate is an async operation, happening in the
             *             hibernate step.
             *
             * @param[in]  id          The id of the entity the component
             *                         belongs to.
             * @param[in]  identifier  The type identifier of the component.
             */
            void
            hibernateComponent(const EntityId& id,
                               const TypeIdentifier& identifier);

            /**
             * @brief      Queues up the removal of the entity with id == id and
             *             all of its components.
             *
             * @note       Remove is an async operation, happening in the remove
             *             step.
             *
             * @param[in]  id    The id of the entity to remove.
             */
            void
            removeEntity(const EntityId& id);

            /**
             * @brief      Queues up the awakening of the entity with id == id
             *             and all of its components.
             *
             * @note       Awake is an async operation, happening in the awake
             *             step.
             *
             * @param[in]  id    The id of the entity to awake.
             */
            void
            awakeEntity(const EntityId& id);

            /**
             * @brief      Queues up the activation of the entity with id == id
             *             and all of its components.
             *
             * @note       Activate is an async operation, happening in the
             *             activate step.
             *
             * @param[in]  id    The id of the entity to activate.
             */
            void
            activateEntity(const EntityId& id);
            
            /**
             * @brief      Queues up the deactivation of the entity with 
             *             id == id and all of its components.
             *
             * @note       Deactivate is an async operation, happening in the
             *             deactivate step.
             *
             * @param[in]  id    The id of the entity to deactivate.
             */
            void
            deactivateEntity(const EntityId& id);

            /**
             * @brief      Queues up the hibernation of the entity with id == id
             *             and all of its components.
             *
             * @note       Hibernate is an async operation, happening in the
             *             hibernate step.
             *
             * @param[in]  id    The id of the entity to hibernate.
             */
            void
            hibernateEntity(const EntityId& id);

            /**
             * @brief      Convenience function, going through all the different steps
             *             in the lifecycle in the following manner:
             *             1. Distribute logic events.
             *             2. Update.
             *             3. Distribute component events.
             *             4. Deactivate components.
             *             5. Hibernate components.
             *             6. Remove components.
             *             7. Create components.
             *             8. Awake components.
             *             9. Activate components.
             *
             * @param[in]  deltaTime  The deltaTime to hand of to the component update.
             */
            void 
            step(const nox::Duration& deltaTime);

            /**
             * @brief      Goes through all the components interested in the
             *             different events that the EntityManager has received
             *             and calls their receiveLogicEvent functions.
             */
            void
            distributeLogicEvents();

            /**
             * @brief      Updates all the components within the EntityManager.
             *
             * @param[in]  deltaTime  The time since this function was last
             *                        called.
             */
            void
            updateStep(const nox::Duration& deltaTime);

            /**
             * @brief      Distributes all the EntityEvents to the designated
             *             entities.
             */
            void
            distributeEntityEvents();
  
            /**
             * @brief      Deactivates all requested components.
             */
            void
            deactivateStep();

            /**
             * @brief      Hibernates all requested components.
             */
            void
            hibernateStep();

            /**
             * @brief      Removes all requested components.
             */
            void
            removeStep();

            /**
             * @brief      Creates all requested components.
             */
            void
            createStep();

            /**
             * @brief      Awakes all requested components.
             */
            void
            awakeStep();

            /**
             * @brief      Activates all requested components.
             */
            void
            activateStep();

            /**
             * @brief      Creates an event with the parameters given to the
             *             function. The event is not added to the EventSystem,
             *             this is done through the sendEntityEvent function.
             *
             * @note       Creating events within receiveEvent functions in a
             *             concurrent environment is legal, and the event will
             *             be parsed in the current frame.
             *
             * @see        nox::ecs::Event
             *
             * @param[in]  eventType   The event type.
             * @param[in]  senderId    The id of the sender.
             * @param[in]  receiverId  The id of the receiver entity. Defaults
             *                         to Event::BROADCAST, i.e. Every entity
             *                         within the system will see the message.
             *
             * @return     An event that can be further customized.
             */
            nox::ecs::Event 
            createEntityEvent(const TypeIdentifier& eventType,
                              const EntityId& senderId,
                              const EntityId& receiverId = nox::ecs::Event::BROADCAST);
            
            /**
             * @brief      Queues up the sending of the entity event.
             *
             * @note       Sending events is an async operation, happening in
             *             the distribute entity events step.
             *
             * @warning    event will be moved from and will therefore be in an
             *             invalid state after this operation.
             *
             * @param[in]  event  The event to send.
             */
            void
            sendEntityEvent(ecs::Event event);

            /**
             * @brief      Receives an event that will in the future be
             *             forwarded to the components.
             *
             * @note       Forwarding the events is an async operation,
             *             happening in the distribute logic events step.
             *
             * @param[in]  event  The event to forward.
             */
            virtual void
            onEvent(const std::shared_ptr<nox::event::Event>& event) override final;

        private:
            /**
             * @brief      Enum wrapper allowing for the use of enums as indexes
             *             without having to cast them constantly, and without
             *             it bleeding into the global namespace.
             */
            struct Transition
            {
                enum Enum : std::size_t
                {
                    DEACTIVATE,
                    HIBERNATE,
                    AWAKE,
                    ACTIVATE,
                    META_COUNT,
                };
            };

            struct ComponentIdentifier
            {
                EntityId id{0};
                TypeIdentifier type{0};
            };

            struct CreationArguments
            {
                EntityId id{0};
                TypeIdentifier type{0};
                Json::Value json{};
                Children children{0, nullptr};
                Parent parent{0, nullptr};
            };


            #if defined(NOX_ENTITYMANAGER_USE_LOCKED_QUEUE)
            template<class T>
            using ContainerType = nox::thread::LockedQueue<T>;
            #else
            template<class T>
            using ContainerType = nox::thread::LockFreeStack<T>;
            #endif

            #if defined(NOX_ENTITYMANAGER_POOL_USE_LOCK_FREE_STACK)
            template<class T>
            using PoolQueueType = nox::thread::LockFreeStack<T>;
            #else
            template<class T>
            using PoolQueueType = nox::thread::LockedQueue<T>;
            #endif

            ComponentCollection&
            getCollection(const TypeIdentifier& identifier);

            Factory factory{*this};

            std::vector<ComponentCollection> components{};

            std::array<ContainerType<ComponentIdentifier>, Transition::META_COUNT> transitionRequests{};

            ContainerType<CreationArguments> creationRequests{};
            ContainerType<ComponentIdentifier> removalRequests{};

            ContainerType<std::shared_ptr<nox::event::Event>> logicEvents{};

            nox::thread::Pool<PoolQueueType> threads{};

            nox::ecs::Event::ArgumentAllocator eventArgumentAllocator{};

            ContainerType<nox::ecs::Event> entityEvents{};

            std::atomic<EntityId> currentEntityId{};

            #ifdef NOX_ECS_LAYERED_EXECUTION_UPDATE
            std::vector<std::vector<std::size_t>> updateExecutionLayers{};
            #endif

            #ifdef NOX_ECS_LAYERED_EXECUTION_ENTITY_EVENTS
            std::vector<std::vector<std::size_t>> entityEventExecutionLayers{};
            #endif

            #ifdef NOX_ECS_LAYERED_EXECUTION_LOGIC_EVENTS
            std::vector<std::vector<std::size_t>> logicEventExecutionLayers{};
            #endif
        };
    }
}

#endif
