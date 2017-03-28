#include <nox/ecs/EntityManager.h>

#include <algorithm>
#include <functional>
#include <set>
#include <utility>

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
    auto typeIdentifierComp = [](const TypeIdentifier& lhs, const TypeIdentifier& rhs)
    {
        return lhs.getValue() < rhs.getValue();
    };

    struct TypeIdentifierComp
    {
        bool operator()(const TypeIdentifier& lhs, const TypeIdentifier& rhs)
        {
            return lhs.getValue() < rhs.getValue();
        }
    };

    using TypeIdentifierSet = std::set<TypeIdentifier, TypeIdentifierComp>;

    std::vector<std::pair<TypeIdentifier, TypeIdentifierSet>> componentAccessLists;

    for (std::size_t i = 0; i < this->components.size(); ++i)
    {
        auto metaInformation = this->components[i].getMetaInformation();

        componentAccessLists.emplace_back(metaInformation.typeIdentifier,
                                          TypeIdentifierSet());

        //Remove self reads
        for (auto itr = std::begin(componentAccessLists.back().second); itr != std::end(componentAccessLists.back().second);)
        {
            if (*itr == componentAccessLists.back().first)
            {
                itr = componentAccessLists.back().second.erase(itr);
            }
            else
            {
                ++itr;
            }
        }
    }

    //Makes sure that the read connection goes both ways for all component types
    //If A reads B, B also needs to read A in this model
    for (const auto& list : componentAccessLists)
    {
        for (const auto& type : list.second)
        {
            const auto itr = std::find_if(std::begin(componentAccessLists),
                                          std::end(componentAccessLists),
                                          [type](const auto& listPair)
                                          { return listPair.first == type; });
            itr->second.insert(list.first);
        }
    }

    std::vector<std::vector<TypeIdentifier>> executionOrder; 
 
    while (!componentAccessLists.empty())
    {     
        //Sort so the list with largest amount of connections to others comes first
        std::sort(std::begin(componentAccessLists),
                  std::end(componentAccessLists),
                  [](const auto& lhs, const auto& rhs)
                  { return lhs.second.size() > rhs.second.size(); });

        //Start of with the component type with the most connections
        TypeIdentifier initialComponent = componentAccessLists.front().first;

        executionOrder.emplace_back();
        executionOrder.back().push_back(initialComponent);

        TypeIdentifierSet connectedComponents(std::begin(componentAccessLists.front().second),
                                              std::end(componentAccessLists.front().second),
                                              TypeIdentifierComp());

        //Need a copy to work with so deleting is not affecting the original
        auto accessListsCopy = componentAccessLists;

        //Done with the starting component, so erase it from the list
        accessListsCopy.erase(std::begin(accessListsCopy));

        while (!accessListsCopy.empty())
        {
            //Remove all component types that has a connection to component types already put in the executionOrder
            for (auto itr = std::begin(accessListsCopy); itr != std::end(accessListsCopy);)
            {
                std::vector<TypeIdentifier> intersection;
                std::set_intersection(std::begin(itr->second), std::end(itr->second),
                                      std::begin(executionOrder.back()), std::end(executionOrder.back()),
                                      std::back_inserter(intersection),
                                      typeIdentifierComp);
                
                if (!intersection.empty())
                {
                    itr = accessListsCopy.erase(itr);
                }
                else
                {
                    ++itr;
                }
            }

            //Ensure not to try to add another component to the executionOrder when there are none left
            if (accessListsCopy.empty())
            {
                break;
            }

            //Lambda less than comparison between which component types has more relations to the connectedComponents
            auto compareFunction = [&connectedComponents, typeIdentifierComp](auto& lhs, auto& rhs)
            { 
                std::vector<TypeIdentifier> intersection;
    
                intersection.clear();
                std::set_intersection(std::begin(lhs.second), std::end(lhs.second),
                                      std::begin(connectedComponents), std::end(connectedComponents),
                                      std::back_inserter(intersection),
                                      typeIdentifierComp);
                const std::size_t lhsSize = intersection.size();
    
                intersection.clear();
                std::set_intersection(std::begin(rhs.second), std::end(rhs.second),
                                      std::begin(connectedComponents), std::end(connectedComponents),
                                      std::back_inserter(intersection),
                                      typeIdentifierComp);
                const std::size_t rhsSize = intersection.size();
    
                return lhsSize > rhsSize;
            };

            //Component type with least amount of new connections to other component types
            const auto minListCopyItr = std::min_element(std::begin(accessListsCopy),
                                                         std::end(accessListsCopy),
                                                         compareFunction);
            const auto minListItr = std::find_if(std::begin(componentAccessLists),
                                                 std::end(componentAccessLists),
                                                 [minListCopyItr](const auto& list)
                                                 { return list.first == minListCopyItr->first; });
            accessListsCopy.erase(std::find_if(std::begin(accessListsCopy),
                                               std::end(accessListsCopy),
                                               [minListItr](const auto& listPair)
                                               { return listPair.first == minListItr->first; }));

            //Add newfound component type to the executionOrder
            executionOrder.back().push_back(minListItr->first);
        }

        //Removes components if there are too many to fit nicely into the threadpool
        const std::size_t overshot = executionOrder.back().size() % threads.threadCount();  
        if (overshot != executionOrder.back().size())
        {
            for (std::size_t i = 0; i < overshot; ++i)
            {
                executionOrder.back().erase(std::end(executionOrder.back()) - 1);
            }
        }

        //Removes the components from the componentAccessLists now placed in the new execution layer
        for (const auto& type : executionOrder.back())
        {
            componentAccessLists.erase(std::find_if(std::begin(componentAccessLists),
                                                    std::end(componentAccessLists),
                                                    [type](auto& pair)
                                                    { return pair.first == type; }));
        }
    }

    //If there are still more components left, add them into separate execution layers
    for (const auto& listPair : componentAccessLists)
    {
        executionOrder.emplace_back();
        executionOrder.back().push_back(listPair.first);
    }

    //Change the executionOrder into the format executionLayers has
    for (const auto& layer : executionOrder)
    {
        this->executionLayers.emplace_back();

        for (const auto& type : layer)
        {
            const auto componentItr = std::find_if(std::begin(this->components),
                                                   std::end(this->components),
                                                   [type](const auto& component)
                                                   { return component.getMetaInformation().typeIdentifier == type; });

            const std::size_t index = std::distance(std::begin(this->components),
                                                    componentItr);

            this->executionLayers.back().push_back(index);
        }
    }

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
