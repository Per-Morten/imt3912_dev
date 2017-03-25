#include <nox/ecs/EntityManager.h>

#include <algorithm>

#include <nox/util/nox_assert.h>
#include <nox/ecs/component/Types.h>

nox::ecs::EntityManager::~EntityManager()
{
    const auto maxId = this->currentEntityId.load(std::memory_order_acquire);
    for (std::size_t i = 0; i < maxId; ++i)
    {
        this->removeEntity(i);
    }

    this->deactivateStep();
    this->hibernateStep();
    this->removeStep();
}

void
nox::ecs::EntityManager::createEntityDefinition(const Json::Value& root)
{
    this->factory.createEntityDefinition(root);
}

void
nox::ecs::EntityManager::registerComponent(const MetaInformation& info)
{
    this->components.push_back(info);
}

void
nox::ecs::EntityManager::configureComponents()
{
    // Temporary solution to test the pool. This will be changed out with the layering algorithm.
    this->threadSafeStop = std::partition(this->components.begin(),
                                          this->components.end(),
                                          [this](const auto& item)
                                          { return item.getMetaInformation().updateAccess == DataAccess::INDEPENDENT; });
}

nox::ecs::EntityId
nox::ecs::EntityManager::createEntity()
{
    const auto newId = this->currentEntityId.fetch_add(EntityId(1), std::memory_order_acq_rel);
    return newId;
}

nox::ecs::EntityId
nox::ecs::EntityManager::createEntity(const std::string& definitionName)
{
    const auto newId = this->currentEntityId.fetch_add(EntityId(1), std::memory_order_acq_rel);
    this->factory.createEntity(newId, definitionName);
    return newId;
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier)
{
    CreationArguments tmp{ id, identifier };
    this->creationQueue.push(std::move(tmp));
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         const Json::Value& value)
{
    CreationArguments tmp{ id, identifier };
    tmp.json = value;
    this->creationQueue.push(std::move(tmp));
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         Children&& children)
{
    CreationArguments tmp{ id, identifier };
    tmp.children = std::move(children);
    this->creationQueue.push(std::move(tmp));
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         Parent&& parent)
{
    CreationArguments tmp{ id, identifier };
    tmp.parent = std::move(parent);
    this->creationQueue.push(std::move(tmp));
}

nox::ecs::ComponentHandle<nox::ecs::Component>
nox::ecs::EntityManager::getComponent(const EntityId& id,
                                      const TypeIdentifier& identifier)
{
    auto& collection = this->getCollection(identifier);
    return collection.getComponent(id);
}

void
nox::ecs::EntityManager::removeComponent(const EntityId& id,
                                         const TypeIdentifier& identifier)
{
    this->removalQueue.push({ id, identifier });
}

void
nox::ecs::EntityManager::awakeComponent(const EntityId& id,
                                        const TypeIdentifier& identifier)
{
    this->transitionQueues[Transition::AWAKE].push({ id, identifier });
}

void
nox::ecs::EntityManager::activateComponent(const EntityId& id,
                                           const TypeIdentifier& identifier)
{
    this->transitionQueues[Transition::ACTIVATE].push({ id, identifier });
}

void
nox::ecs::EntityManager::deactivateComponent(const EntityId& id,
                                             const TypeIdentifier& identifier)
{
    this->transitionQueues[Transition::DEACTIVATE].push({ id, identifier });
}

void
nox::ecs::EntityManager::hibernateComponent(const EntityId& id, 
                                            const TypeIdentifier& identifier)
{
    this->transitionQueues[Transition::HIBERNATE].push({ id, identifier });
}

void
nox::ecs::EntityManager::removeEntity(const EntityId& id)
{
    for (const auto& item : this->components)
    {
        this->removeComponent(id, item.getTypeIdentifier());
    }
}

void
nox::ecs::EntityManager::awakeEntity(const EntityId& id)
{
    for (const auto& item : this->components)
    {
        this->awakeComponent(id, item.getTypeIdentifier());
    }
}

void
nox::ecs::EntityManager::activateEntity(const EntityId& id)
{
    for (const auto& item : this->components)
    {
        this->activateComponent(id, item.getTypeIdentifier());
    }
}

void
nox::ecs::EntityManager::deactivateEntity(const EntityId& id)
{
    for (const auto& item : this->components)
    {
        this->deactivateComponent(id, item.getTypeIdentifier());
    }
}

void
nox::ecs::EntityManager::hibernateEntity(const EntityId& id)
{
    for (const auto& item : this->components)
    {
        this->hibernateComponent(id, item.getTypeIdentifier());
    }
}

void
nox::ecs::EntityManager::step(const nox::Duration& deltaTime)
{
    this->distributeLogicEvents();
    this->updateStep(deltaTime);
    this->distributeEntityEvents();
    this->deactivateStep();
    this->hibernateStep();
    this->removeStep();
    this->createStep();
    this->awakeStep();
    this->activateStep();
}

void
nox::ecs::EntityManager::distributeLogicEvents()
{
    std::shared_ptr<nox::event::Event> event{};
    while (this->logicEvents.pop(event))
    {
        for (auto& collection : this->components)
        {
            collection.receiveLogicEvent(event);
        }
    }
}

void
nox::ecs::EntityManager::updateStep(const nox::Duration& deltaTime)
{
    for (auto itr = this->components.begin(); itr != this->threadSafeStop; ++itr)
    {
        this->threads.addTask([this, itr, deltaTime](){ itr->update(deltaTime); });
    }
    this->threads.wait();

    for (auto itr = this->threadSafeStop; itr != this->components.end(); ++itr)
    {
        itr->update(deltaTime);
    }
}

void
nox::ecs::EntityManager::distributeEntityEvents()
{
    // Temp event, only needed for the popping.
    Event event(&eventArgumentAllocator, {0}, 0, 0);
    while (this->entityEventSystem.pop(event))
    {
        for (auto& collection : this->components)
        {
            collection.receiveEntityEvent(event);
        }
    }
    this->entityEventSystem.clear();
    this->eventArgumentAllocator.clear();
}

void
nox::ecs::EntityManager::deactivateStep()
{
    ComponentIdentifier identifier;
    while (this->transitionQueues[Transition::DEACTIVATE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.deactivate(identifier.id);
    }
}

void
nox::ecs::EntityManager::hibernateStep()
{
    ComponentIdentifier identifier;
    while (this->transitionQueues[Transition::HIBERNATE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.hibernate(identifier.id);
    }
}

void
nox::ecs::EntityManager::removeStep()
{
    ComponentIdentifier identifier;
    while (this->removalQueue.pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.remove(identifier.id);
    }
}

void
nox::ecs::EntityManager::createStep()
{
    CreationArguments identifier;
    while (this->creationQueue.pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);

        if (identifier.type == ecs::component_types::CHILDREN)
        {
            Children& child = identifier.children;
            collection.adopt(child);
        } 
        else if (identifier.type == ecs::component_types::PARENT)
        {
            Parent& parent = identifier.parent;
            collection.adopt(parent);
        }
        else
        {
            collection.create(identifier.id, this);
            const Json::Value& jsonValue = identifier.json;

            if (jsonValue.isNull())
            {
                collection.initialize(identifier.id, jsonValue);
            }
        }
    }
}

void
nox::ecs::EntityManager::awakeStep()
{
    ComponentIdentifier identifier;
    while (this->transitionQueues[Transition::AWAKE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.awake(identifier.id);
    }
}

void
nox::ecs::EntityManager::activateStep()
{
    ComponentIdentifier identifier;
    while (this->transitionQueues[Transition::ACTIVATE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.activate(identifier.id);
    }
}

nox::ecs::Event
nox::ecs::EntityManager::createEntityEvent(const TypeIdentifier& eventType,
                                           const EntityId& senderId,
                                           const EntityId& receiverId)
{
    return std::move(Event(&eventArgumentAllocator,
                           eventType,
                           senderId,
                           receiverId));
}

void
nox::ecs::EntityManager::sendEntityEvent(ecs::Event event)
{
    this->entityEventSystem.push(std::move(event));
}

void
nox::ecs::EntityManager::onEvent(const std::shared_ptr<nox::event::Event>& event)
{
    logicEvents.push(event);
}

nox::ecs::ComponentCollection&
nox::ecs::EntityManager::getCollection(const TypeIdentifier& identifier)
{
    auto collection = std::find_if(std::begin(this->components),
                                   std::end(this->components),
                                   [&identifier](const auto& item)
                                   { return item.getTypeIdentifier() == identifier; });
    NOX_ASSERT(collection != std::end(this->components), "Illegal identifier, collection not found!\n");

    return *collection;
}
