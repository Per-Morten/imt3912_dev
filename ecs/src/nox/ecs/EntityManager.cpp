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
    // To be implemented sprint05. Needed to avoid linker issues.
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
    this->componentCreationQueue.push_back({ id, identifier });
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         const Json::Value& value)
{
    this->componentCreationQueue.push_back({ id, identifier, value });
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         Children children)
{
    this->componentCreationQueue.push_back({ id, identifier, std::move(children) });
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         Parent parent)
{
    this->componentCreationQueue.push_back({ id, identifier, std::move(parent) });
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
    while (!this->entityEvents.empty())
    {
        auto event = std::move(entityEvents.front());
        entityEvents.pop();

        for (auto& collection : this->components)
        {
            collection.receiveEntityEvent(event);
        }
    }
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
        const auto componentIdentifier = std::move(this->componentRemovalQueue.front());
        this->componentRemovalQueue.pop_front();

        auto& collection = this->getCollection(componentIdentifier.identifier);
        collection.remove(componentIdentifier.id);
    }
}

void
nox::ecs::EntityManager::createStep()
{
    while (!this->componentCreationQueue.empty())
    {
        auto componentIdentifier = std::move(this->componentCreationQueue.front());
        this->componentCreationQueue.pop_front();

        auto& collection = this->getCollection(componentIdentifier.identifier);

        if (componentIdentifier.identifier == ecs::component_types::CHILDREN)
        {
            Children& child = boost::get<Children>(componentIdentifier.initValue);
            collection.adopt(child);
        } 
        else if (componentIdentifier.identifier == ecs::component_types::PARENT)
        {
            Parent& parent = boost::get<Parent>(componentIdentifier.initValue);
            collection.adopt(parent);
        }
        else
        {
            collection.create(componentIdentifier.id, this);
            const Json::Value* jsonValue = boost::get<Json::Value>(&componentIdentifier.initValue);

            if (jsonValue != nullptr)
            {
                collection.initialize(componentIdentifier.id, *jsonValue);
            }
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
nox::ecs::EntityManager::sendEntityEvent(ecs::Event event)
{
    this->entityEvents.push(std::move(event));
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
