#include <nox/ecs/UniquePtrComponentCollection.h>

#include <cstdlib>
#include <nox/util/pms_debug.h>

#ifdef NOX_ECS_COMPONENT_UNIQUE_PTR_VIRTUAL

nox::ecs::UniquePtrComponentCollection::UniquePtrComponentCollection(const MetaInformation& info)
    : info(info)
{
}

void
nox::ecs::UniquePtrComponentCollection::create(const EntityId& id,
                                             EntityManager* manager)
{
    const auto before = this->findBefore(id);
    this->components.insert(before, std::move(this->info.virtualCreate(id, manager)));
}

void
nox::ecs::UniquePtrComponentCollection::adopt(Component& component)
{
    const auto before = this->findBefore(component.id);

    this->components.insert(before, std::move(this->info.virtualMoveCreate(component)));
}

void
nox::ecs::UniquePtrComponentCollection::initialize(const EntityId& id,
                                          const Json::Value& value)
{
    auto target = this->find(id);
    if (target != std::end(this->components))
    {
        (*target)->initialize(value);
    }
}

void
nox::ecs::UniquePtrComponentCollection::awake(const EntityId& id)
{
    auto target = this->find(id);
    if (target != std::end(this->components))
    {
        (*target)->awake();
    }
}

void
nox::ecs::UniquePtrComponentCollection::activate(const EntityId& id)
{
    auto target = this->find(id);
    if (target != std::end(this->components))
    {
        (*target)->activate();
    }
}

void
nox::ecs::UniquePtrComponentCollection::deactivate(const EntityId& id)
{
    auto target = this->find(id);
    if (target != std::end(this->components))
    {
        (*target)->deactivate();
    }
}

void
nox::ecs::UniquePtrComponentCollection::hibernate(const EntityId& id)
{
    auto target = this->find(id);
    if (target != std::end(this->components))
    {
        (*target)->hibernate();
    }
}

void
nox::ecs::UniquePtrComponentCollection::remove(const EntityId& id)
{
    auto target = this->find(id);
    if (target != std::end(this->components))
    {
        this->components.erase(target);
        this->gen++;
    }
}

void
nox::ecs::UniquePtrComponentCollection::update(const nox::Duration& duration)
{
    for (auto& item : this->components)
    {
        item->update(duration);
    }
}

void
nox::ecs::UniquePtrComponentCollection::receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
{
    for (auto& item : this->components)
    {
        item->receiveLogicEvent(event);
    }
}

void
nox::ecs::UniquePtrComponentCollection::receiveEntityEvent(const ecs::Event& event)
{
    if (event.getReceiver() == ecs::Event::BROADCAST)
    {
        for (auto& item : this->components)
        {
            item->receiveEntityEvent(event);
        }
    }
    else
    {
        auto target = this->find(event.getReceiver());
        if (target != std::end(this->components))
        {
            (*target)->receiveEntityEvent(event);
        }
    }
}

std::size_t
nox::ecs::UniquePtrComponentCollection::count() const
{
    return this->components.size();
}

nox::ecs::ComponentHandle<nox::ecs::Component>
nox::ecs::UniquePtrComponentCollection::getComponent(const EntityId& id)
{
    auto itr = this->find(id);
    auto component = (itr != std::end(this->components)) ? itr->get() : nullptr;
    ComponentHandle<Component> handle(id,
                                      component,
                                      this->gen,
                                      this);
    return handle;
}

std::size_t
nox::ecs::UniquePtrComponentCollection::getGeneration() const
{
    return this->gen;
}

const nox::ecs::TypeIdentifier&
nox::ecs::UniquePtrComponentCollection::getTypeIdentifier() const
{
    return this->info.typeIdentifier;
}

const nox::ecs::MetaInformation&
nox::ecs::UniquePtrComponentCollection::getMetaInformation() const
{
    return this->info;
}

nox::ecs::UniquePtrComponentCollection::Components::iterator
nox::ecs::UniquePtrComponentCollection::find(const EntityId& id)
{
    const auto component = this->findBefore(id);
    if (component != std::end(this->components) &&
        (*component)->id > id)
    {
        return std::end(this->components);
    }
    return component;
}

nox::ecs::UniquePtrComponentCollection::Components::const_iterator
nox::ecs::UniquePtrComponentCollection::find(const EntityId& id) const
{
    const auto component = this->findBefore(id);
    if (component != std::cend(this->components) &&
        (*component)->id > id)
    {
        return std::cend(this->components);
    }
    return component;
}

nox::ecs::UniquePtrComponentCollection::Components::iterator
nox::ecs::UniquePtrComponentCollection::findBefore(const EntityId& id)
{
    return std::lower_bound(std::begin(this->components),
                            std::end(this->components),
                            id,
                            [](const auto& element, const auto& value)
                            { return element->id < value; });
}

nox::ecs::UniquePtrComponentCollection::Components::const_iterator
nox::ecs::UniquePtrComponentCollection::findBefore(const EntityId& id) const
{
    return std::lower_bound(std::cbegin(this->components),
                            std::cend(this->components),
                            id,
                            [](const auto& element, const auto& value)
                            { return element->id < value; });
}

#endif
