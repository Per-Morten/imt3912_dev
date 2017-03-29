#include <nox/ecs/EntityManager.h>

#include <algorithm>
#include <functional>
#include <set>
#include <utility>

#include <nox/util/nox_assert.h>
#include <nox/ecs/component/Types.h>

namespace
{
    namespace local
    {
        //Utilizing using namespace to avoid having to rewrite a lot of the code
        using namespace nox::ecs;
        
        struct TypeIdentifierComp
        {
            bool operator()(const TypeIdentifier& lhs, const TypeIdentifier& rhs)
            {
                return lhs.getValue() < rhs.getValue();
            }
        };

        using TypeIdentifierSet = std::set<TypeIdentifier, TypeIdentifierComp>;

        struct ComponentCollectionInfo
        {
            TypeIdentifier type{0};
            DataAccess access{DataAccess::UNKNOWN};
            TypeIdentifierSet connectionSet;
        };

        std::vector<std::vector<std::size_t>> 
        parseExecutionOrder(const std::vector<std::vector<TypeIdentifier>>& executionOrder,
                            const std::vector<ComponentCollection>& collections)
        {
            std::vector<std::vector<std::size_t>> executionLayers;

            for (const auto& layer : executionOrder)
            {
                executionLayers.emplace_back();

                for (const auto& type : layer)
                {
                    const auto componentItr = std::find_if(std::cbegin(collections),
                                                           std::cend(collections),
                                                           [type](const auto& component)
                                                           { return component.getMetaInformation().typeIdentifier == type; });
        
                    const std::size_t index = std::distance(std::cbegin(collections),
                                                            componentItr);
        
                    executionLayers.back().push_back(index);
                }
            }

            return executionLayers;
        }

        void
        initializeData(std::vector<ComponentCollectionInfo>& componentAccessLists,
                       std::vector<std::vector<TypeIdentifier>>& executionOrder,
                       const std::vector<ComponentCollection>& collections)
        {
            for (const auto& collection : collections)
            {
                auto metaInformation = collection.getMetaInformation();
                
                ComponentCollectionInfo newComponent;
                newComponent.type = metaInformation.typeIdentifier;
                newComponent.access = metaInformation.updateAccess;
                newComponent.connectionSet.insert(metaInformation.updateDependencies.cbegin(),
                                                  metaInformation.updateDependencies.cend());

                componentAccessLists.push_back(newComponent);

                //Remove self reads
                auto& list = componentAccessLists.back();
                for (auto itr = std::begin(list.connectionSet); itr != std::end(list.connectionSet);)
                {
                    itr = (*itr == list.type) ? list.connectionSet.erase(itr) : std::next(itr);
                }

                //Ensure independent component types have no read connections
                if (newComponent.access == DataAccess::INDEPENDENT)
                {
                    newComponent.connectionSet.clear();
                }
                //Immediately place read_write and unknown component types into their own separate execution layer
                else if (newComponent.access == DataAccess::READ_WRITE || 
                         newComponent.access == DataAccess::UNKNOWN)
                {
                    executionOrder.emplace_back();
                    executionOrder.back().push_back(newComponent.type);
                    componentAccessLists.erase(std::end(componentAccessLists) - 1);
                }
            }
        
            //Makes sure that the read connection goes both ways for all component types
            //If A reads B, B also needs to read A in this model
            for (const auto& collection : componentAccessLists)
            {
                for (const auto& type : collection.connectionSet)
                {
                    const auto itr = std::find_if(std::begin(componentAccessLists),
                                                  std::end(componentAccessLists),
                                                  [type](const auto& collection)
                                                  { return collection.type == type; });
                    itr->connectionSet.insert(collection.type);
                }
            }
        }

        std::vector<std::vector<std::size_t>> 
        createExecutionLayers(const std::vector<ComponentCollection>& collections,
                              std::size_t threadCount)
        {        
            std::vector<ComponentCollectionInfo> componentAccessLists;
            std::vector<std::vector<TypeIdentifier>> executionOrder; 
        
            //Collect and set up the data properly for the algorithm to use
            initializeData(componentAccessLists, executionOrder, collections);
        
            while (!componentAccessLists.empty())
            {     
                //Sort so the list with largest amount of connections to others comes first
                std::sort(std::begin(componentAccessLists),
                          std::end(componentAccessLists),
                          [](const auto& lhs, const auto& rhs)
                          { return lhs.connectionSet.size() > rhs.connectionSet.size(); });
        
                //Start of with the component type with the most connections
                TypeIdentifier initialComponent = componentAccessLists.front().type;
        
                executionOrder.emplace_back();
                executionOrder.back().push_back(initialComponent);
        
                TypeIdentifierSet connectedComponents(std::cbegin(componentAccessLists.front().connectionSet),
                                                      std::cend(componentAccessLists.front().connectionSet));
        
                //Need a copy to work with so deleting is not affecting the original
                auto accessListsCopy = componentAccessLists;
        
                //Done with the starting component, so erase it from the list
                accessListsCopy.erase(std::cbegin(accessListsCopy));
        
                while (!accessListsCopy.empty())
                {
                    //Remove all component types that has a connection to component types already put in the executionOrder
                    for (auto itr = std::begin(accessListsCopy); itr != std::end(accessListsCopy);)
                    {
                        std::vector<TypeIdentifier> intersection;
                        std::set_intersection(std::begin(itr->connectionSet), std::end(itr->connectionSet),
                                              std::begin(executionOrder.back()), std::end(executionOrder.back()),
                                              std::back_inserter(intersection),
                                              TypeIdentifierComp());

                        itr = (!intersection.empty()) ? accessListsCopy.erase(itr) : std::next(itr);
                    }
        
                    //Ensure not to try to add another component to the executionOrder when there are none left
                    if (accessListsCopy.empty())
                    {
                        break;
                    }
        
                    //Lambda less than comparison between which component types has more relations to the connectedComponents
                    const auto compareFunction = [&connectedComponents](const auto& lhs, const auto& rhs)
                    { 
                        const auto intersectionSize = [&connectedComponents](const auto& item)
                        {
                            std::vector<TypeIdentifier> intersection;
                            std::set_intersection(std::cbegin(item), std::cend(item),
                                                  std::cbegin(connectedComponents), std::cend(connectedComponents),
                                                  std::back_inserter(intersection),
                                                  TypeIdentifierComp());
        
                            return intersection.size();
                        };
        
                        return intersectionSize(lhs.connectionSet) > intersectionSize(rhs.connectionSet);
                    };
        
                    //Component type with least amount of new connections to other component types
                    const auto minListCopyItr = std::min_element(std::cbegin(accessListsCopy),
                                                                 std::cend(accessListsCopy),
                                                                 compareFunction);
                    const auto minListItr = std::find_if(std::cbegin(componentAccessLists),
                                                         std::cend(componentAccessLists),
                                                         [minListCopyItr](const auto& collection)
                                                         { return collection.type == minListCopyItr->type; });
                    
                    //Add newfound component type to the executionOrder
                    executionOrder.back().push_back(minListItr->type);
                    connectedComponents.insert(std::begin(minListItr->connectionSet),
                                               std::end(minListItr->connectionSet));
                    accessListsCopy.erase(std::find_if(std::cbegin(accessListsCopy),
                                                       std::cend(accessListsCopy),
                                                       [minListItr](const auto& collection)
                                                       { return collection.type == minListItr->type; }));

                    std::sort(std::begin(executionOrder.back()),
                              std::end(executionOrder.back()),
                              [](const auto& lhs, const auto& rhs)
                              { return lhs.getValue() < rhs.getValue(); });
        
                }
        
                //Removes components if there are too many to fit nicely into the threadpool
                const std::size_t overshot = executionOrder.back().size() % threadCount;  
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
                    componentAccessLists.erase(std::find_if(std::cbegin(componentAccessLists),
                                                            std::cend(componentAccessLists),
                                                            [type](const auto& collection)
                                                            { return collection.type == type; }));
                }
            }
        
            //If there are still more components left, add them into separate execution layers
            for (const auto& info : componentAccessLists)
            {
                executionOrder.emplace_back();
                executionOrder.back().push_back(info.type);
            }
        
            //Change the executionOrder into the format executionLayers should have
            return parseExecutionOrder(executionOrder, collections);
        }
    }
}

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
    this->executionLayers = local::createExecutionLayers(this->components, this->threads.threadCount());
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
    this->creationRequests.push(std::move(tmp));
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         const Json::Value& value)
{
    CreationArguments tmp{ id, identifier };
    tmp.json = value;
    this->creationRequests.push(std::move(tmp));
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         Children&& children)
{
    CreationArguments tmp{ id, identifier };
    tmp.children = std::move(children);
    this->creationRequests.push(std::move(tmp));
}

void
nox::ecs::EntityManager::assignComponent(const EntityId& id,
                                         const TypeIdentifier& identifier,
                                         Parent&& parent)
{
    CreationArguments tmp{ id, identifier };
    tmp.parent = std::move(parent);
    this->creationRequests.push(std::move(tmp));
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
    this->removalRequests.push({ id, identifier });
}

void
nox::ecs::EntityManager::awakeComponent(const EntityId& id,
                                        const TypeIdentifier& identifier)
{
    this->transitionRequests[Transition::AWAKE].push({ id, identifier });
}

void
nox::ecs::EntityManager::activateComponent(const EntityId& id,
                                           const TypeIdentifier& identifier)
{
    this->transitionRequests[Transition::ACTIVATE].push({ id, identifier });
}

void
nox::ecs::EntityManager::deactivateComponent(const EntityId& id,
                                             const TypeIdentifier& identifier)
{
    this->transitionRequests[Transition::DEACTIVATE].push({ id, identifier });
}

void
nox::ecs::EntityManager::hibernateComponent(const EntityId& id, 
                                            const TypeIdentifier& identifier)
{
    this->transitionRequests[Transition::HIBERNATE].push({ id, identifier });
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
    this->logicEvents.clear();
}

void
nox::ecs::EntityManager::updateStep(const nox::Duration& deltaTime)
{
    for (const auto& layer : this->executionLayers)
    {
        for (const auto& item : layer)
        {
            this->threads.addTask([this, item, deltaTime]()
                                  { this->components[item].update(deltaTime); });
        }
        this->threads.wait();
    }
}

void
nox::ecs::EntityManager::distributeEntityEvents()
{
    // Temp event, only needed for the popping.
    Event event(&eventArgumentAllocator, {0}, 0, 0);
    while (this->entityEvents.pop(event))
    {
        for (auto& collection : this->components)
        {
            collection.receiveEntityEvent(event);
        }
    }
    // Hack, done to ensure that event is destroyed 
    // before the eventArgumentAllocator is cleared.
    event.~Event();

    this->entityEvents.clear();
    this->eventArgumentAllocator.clear();
}

void
nox::ecs::EntityManager::deactivateStep()
{
    ComponentIdentifier identifier;
    while (this->transitionRequests[Transition::DEACTIVATE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.deactivate(identifier.id);
    }
    this->transitionRequests[Transition::DEACTIVATE].clear();
}

void
nox::ecs::EntityManager::hibernateStep()
{
    ComponentIdentifier identifier;
    while (this->transitionRequests[Transition::HIBERNATE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.hibernate(identifier.id);
    }
    this->transitionRequests[Transition::HIBERNATE].clear();
}

void
nox::ecs::EntityManager::removeStep()
{
    ComponentIdentifier identifier;
    while (this->removalRequests.pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.remove(identifier.id);
    }
    this->removalRequests.clear();
}

void
nox::ecs::EntityManager::createStep()
{
    CreationArguments identifier;
    while (this->creationRequests.pop(identifier))
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
    this->creationRequests.clear();
}

void
nox::ecs::EntityManager::awakeStep()
{
    ComponentIdentifier identifier;
    while (this->transitionRequests[Transition::AWAKE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.awake(identifier.id);
    }
    this->transitionRequests[Transition::AWAKE].clear();
}

void
nox::ecs::EntityManager::activateStep()
{
    ComponentIdentifier identifier;
    while (this->transitionRequests[Transition::ACTIVATE].pop(identifier))
    {
        auto& collection = this->getCollection(identifier.type);
        collection.activate(identifier.id);
    }
    this->transitionRequests[Transition::ACTIVATE].clear();
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
    this->entityEvents.push(std::move(event));
}

void
nox::ecs::EntityManager::onEvent(const std::shared_ptr<nox::event::Event>& event)
{
    this->logicEvents.push(event);
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
