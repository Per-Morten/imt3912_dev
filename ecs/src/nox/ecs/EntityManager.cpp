#include <nox/ecs/EntityManager.h>

#include <algorithm>

#include <nox/util/nox_assert.h>

nox::ecs::EntityManager::~EntityManager()
{
    for (const auto item : this->activeIds)
    {
        this->removeEntity(item);
    }

    this->deactivateStep();
    this->hibernateStep();
    this->removeStep();
}

void
nox::ecs::EntityManager::registerComponent(const MetaInformation& info)
{
    this->components.push_back(info);
}

void
nox::ecs::EntityManager::configureComponents()
{
    // To be implemented sprint05. Needed to avoid linker issues.
}

nox::ecs::EntityId
nox::ecs::EntityManager::createEntity()
{
    const auto newId = this->currentEntityId++;
    this->activeIds.push_back(newId);
    return newId;
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier)
{
    this->componentCreationQueue.push_back({ id, identifier });
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         const Json::Value& value)
{
    this->componentCreationQueue.push_back({ id, identifier, value });
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
    this->componentRemovalQueue.push_back({ id, identifier });
}


void
nox::ecs::EntityManager::awakeComponent(const EntityId& id,
                                        const TypeIdentifier& identifier)
{
    this->componentTransitionQueue.push_back({ id, identifier, Transition::AWAKE });    
}

void
nox::ecs::EntityManager::activateComponent(const EntityId& id,
                                           const TypeIdentifier& identifier)
{
    this->componentTransitionQueue.push_back({ id, identifier, Transition::ACTIVATE });
}

void
nox::ecs::EntityManager::deactivateComponent(const EntityId& id,
                                             const TypeIdentifier& identifier)
{
    this->componentTransitionQueue.push_back({ id, identifier, Transition::DEACTIVATE });
}

void
nox::ecs::EntityManager::hibernateComponent(const EntityId& id, 
                                            const TypeIdentifier& identifier)
{
    this->componentTransitionQueue.push_back({ id, identifier, Transition::HIBERNATE });
}

void
nox::ecs::EntityManager::removeEntity(const EntityId& id)
{
    for (const auto& item : this->components)
    {
        this->removeComponent(id, item.getTypeIdentifier());
    }
    this->entityRemovalQueue.push(id);
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
    while (!this->logicEvents.empty())
    {
        auto event = this->logicEvents.front();
        this->logicEvents.pop();

        for (auto& collection : this->components)
        {
            collection.receiveLogicEvent(event);
        }
    }
}

void
nox::ecs::EntityManager::updateStep(const nox::Duration& deltaTime)
{
    for (auto& collection : this->components)
    {
        collection.update(deltaTime);
    }
}

void
nox::ecs::EntityManager::distributeEntityEvents()
{
    // To be implemented sprint05 
}

void
nox::ecs::EntityManager::deactivateStep()
{
    const auto end = std::partition(std::begin(this->componentTransitionQueue),
                                    std::end(this->componentTransitionQueue),
                                    [](const auto& item)
                                    { return item.transition == Transition::DEACTIVATE; });

    while (std::begin(this->componentTransitionQueue) != end)
    {
        const auto transition = this->componentTransitionQueue.front();
        this->componentTransitionQueue.pop_front(); 

        auto& collection = this->getCollection(transition.identifier);
        collection.deactivate(transition.id);
    }
}

void
nox::ecs::EntityManager::hibernateStep()
{
    const auto end = std::partition(std::begin(this->componentTransitionQueue),
                                    std::end(this->componentTransitionQueue),
                                    [](const auto& item)
                                    { return item.transition == Transition::HIBERNATE; });

    while (std::begin(this->componentTransitionQueue) != end)
    {
        const auto transition = this->componentTransitionQueue.front();
        this->componentTransitionQueue.pop_front(); 

        auto& collection = this->getCollection(transition.identifier);
        collection.hibernate(transition.id);
    }
}

void
nox::ecs::EntityManager::removeStep()
{
    while (!this->componentRemovalQueue.empty())
    {
        const auto componentIdentifier = this->componentRemovalQueue.front();
        this->componentRemovalQueue.pop_front();

        auto& collection = this->getCollection(componentIdentifier.identifier);
        collection.remove(componentIdentifier.id);
    }

    while (!this->entityRemovalQueue.empty())
    {
        const auto entityId = this->entityRemovalQueue.front();
        this->entityRemovalQueue.pop();

        this->activeIds.erase(std::remove(std::begin(this->activeIds),
                                          std::end(this->activeIds),
                                          entityId),
                              std::end(this->activeIds));
    }
}

void
nox::ecs::EntityManager::createStep()
{
    while (!this->componentCreationQueue.empty())
    {
        const auto componentIdentifier = this->componentCreationQueue.front();
        this->componentCreationQueue.pop_front();

        auto& collection = this->getCollection(componentIdentifier.identifier);
        collection.create(componentIdentifier.id);
        if (!componentIdentifier.json.empty())
        {
            collection.initialize(componentIdentifier.id, componentIdentifier.json);
        }
    }
}

void
nox::ecs::EntityManager::awakeStep()
{
    const auto end = std::partition(std::begin(this->componentTransitionQueue),
                                    std::end(this->componentTransitionQueue),
                                    [](const auto& item)
                                    { return item.transition == Transition::AWAKE; });

    while (std::begin(this->componentTransitionQueue) != end)
    {
        const auto transition = this->componentTransitionQueue.front();
        this->componentTransitionQueue.pop_front(); 

        auto& collection = this->getCollection(transition.identifier);
        collection.awake(transition.id);
    }
}

void
nox::ecs::EntityManager::activateStep()
{
    const auto end = std::partition(std::begin(this->componentTransitionQueue),
                                    std::end(this->componentTransitionQueue),
                                    [](const auto& item)
                                    { return item.transition == Transition::ACTIVATE; });

    while (std::begin(this->componentTransitionQueue) != end)
    {
        const auto transition = this->componentTransitionQueue.front();
        this->componentTransitionQueue.pop_front(); 

        auto& collection = this->getCollection(transition.identifier);
        collection.activate(transition.id);
    }
}

void
nox::ecs::EntityManager::sendEntityEvent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         const ecs::Event& event)
{
    // To be implemented sprint05.
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
