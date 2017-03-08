#include <nox/ecs/ComponentCollection.h>

#include <cstdlib>

nox::ecs::ComponentCollection::ComponentCollection(const MetaInformation& info)
    : info(info)
    , gen(0)                                           
    , active(static_cast<Byte*>(std::malloc((GROWTH_FACTOR + 1) * info.size))) // + 1 for swap area.
    , inactive(active)
    , hibernating(active)
    , memory(active)
    , cap(active + (GROWTH_FACTOR * info.size))
{
}

nox::ecs::ComponentCollection::ComponentCollection(ComponentCollection&& source)
    : info(std::move(source.info))
    , gen(std::move(source.gen))
    , active(std::move(source.active))
    , inactive(std::move(source.inactive))
    , hibernating(std::move(source.hibernating))
    , memory(std::move(source.memory))
    , cap(std::move(source.cap))
{
    source.active = nullptr;
    source.inactive = nullptr;
    source.hibernating = nullptr;
    source.memory = nullptr;
    source.cap = nullptr;
    source.gen = 0;
}

nox::ecs::ComponentCollection&
nox::ecs::ComponentCollection::operator=(ComponentCollection&& source)
{
    if (this != &source)
    {
        this->destroyRange(active, memory);
        std::free(active);
        this->info = std::move(source.info);
        this->gen = std::move(source.gen);
        this->active = std::move(source.active);
        this->inactive = std::move(source.inactive);
        this->hibernating = std::move(source.hibernating);
        this->memory = std::move(source.memory);
        this->cap = std::move(source.cap);
        
        source.active = nullptr;
        source.inactive = nullptr;
        source.hibernating = nullptr;
        source.memory = nullptr;
        source.cap = nullptr;
        source.gen = 0;
    }

    return *this;
}

nox::ecs::ComponentCollection::~ComponentCollection()
{
    auto begin = this->active;
    const auto end = this->memory;

    while (begin != end)
    {
        auto ptr = this->cast(begin);
        this->info.destruct(ptr);
        begin += this->info.size;
    }

    std::free(this->active);
    this->active = nullptr;
    this->inactive = nullptr;
    this->hibernating = nullptr;
    this->memory = nullptr;
    this->cap = nullptr;
}

void
nox::ecs::ComponentCollection::create(const EntityId& id, 
                                      EntityManager* manager)
{
    if (this->size() >= this->capacity())
    {
        this->reallocate();
    }

    this->info.construct(this->cast(this->memory), id, manager);
    this->memory += this->info.size;
}

void
nox::ecs::ComponentCollection::adopt(Component& component)
{
    if (this->size() >= this->capacity())
    {
        this->reallocate();
    }
    this->info.moveConstruct(this->cast(this->memory), &component);
    this->memory += this->info.size;
}

void
nox::ecs::ComponentCollection::initialize(const EntityId& id,
                                          const Json::Value& value)
{
    if (this->info.initialize)
    {
        auto component = this->find(this->hibernating,
                                    this->memory, 
                                    id);
        
        if (component)
        {
            this->info.initialize(component, value);
        }
    }
}

void
nox::ecs::ComponentCollection::awake(const EntityId& id)
{
    auto component = this->find(this->hibernating,
                                this->memory,
                                id);

    if (component)
    {
        auto ptr = this->cast(this->hibernating);
        this->hibernating += this->info.size;
        this->swap(component, ptr);

        if (this->info.awake)
        {
            this->info.awake(component);
        }
    }
}

void
nox::ecs::ComponentCollection::activate(const EntityId& id)
{
    auto component = this->find(this->inactive,
                                this->hibernating,
                                id);

    if (component)
    {
        auto ptr = this->cast(this->inactive);
        this->inactive += this->info.size;
        this->swap(component, ptr);

        if (this->info.activate)
        {
            this->info.activate(component);
        }
    }
}

void
nox::ecs::ComponentCollection::deactivate(const EntityId& id)
{
    auto component = this->find(this->active,
                                this->inactive,
                                id);

    if (component)
    {
        this->inactive -= this->info.size;
        auto ptr = this->cast(this->inactive);
        this->swap(component, ptr);
        
        if (this->info.deactivate)
        {
            this->info.deactivate(ptr);
        }
    }
}

void
nox::ecs::ComponentCollection::hibernate(const EntityId& id)
{
    auto component = this->find(this->inactive,
                                this->hibernating,
                                id);

    if (component)
    {
        this->hibernating -= this->info.size;
        auto ptr = this->cast(this->hibernating);
        this->swap(component, ptr);

        if (this->info.hibernate)
        {
            this->info.hibernate(ptr);
        }
    }
}

void
nox::ecs::ComponentCollection::remove(const EntityId& id)
{
    auto component = this->find(this->hibernating,
                                this->memory,
                                id);

    if (component)
    {
        this->memory -= this->info.size;
        auto ptr = this->cast(this->memory);

        this->info.moveAssign(component, ptr);
        this->info.destruct(ptr);

        this->gen++; 
    }
}

void
nox::ecs::ComponentCollection::update(const nox::Duration& duration)
{
    if (this->info.update)
    {
        auto begin = this->cast(this->active);
        auto end = this->cast(this->inactive);
    
        this->info.update(begin, end, duration);
    }
}

void
nox::ecs::ComponentCollection::receiveLogicEvent(const std::shared_ptr<nox::event::Event>& event)
{
    if (this->info.receiveLogicEvent)
    {
        auto begin = this->cast(this->active);
        auto end = this->cast(this->memory);

        this->info.receiveLogicEvent(begin, end, event);
    }
}

void
nox::ecs::ComponentCollection::receiveEntityEvent(const ecs::Event& event)
{
    if (this->info.receiveEntityEvent)
    {
        if (event.getReceiver() == ecs::Event::BROADCAST)
        {
            auto begin = this->cast(this->active);
            auto end = this->cast(this->memory);
            this->info.receiveEntityEvent(begin, end, event);
        }
        else
        {
            auto component = this->find(this->active, this->memory, event.getReceiver());
            if (component)
            {
                // Ugly I know. However I must increment the bytes the correct number. 
                // And I can't do that without casting it over to bytes.
                auto end = reinterpret_cast<Component*>(reinterpret_cast<Byte*>(component) + this->info.size);
                this->info.receiveEntityEvent(component, end, event);
            }
        }
    }
}

std::size_t
nox::ecs::ComponentCollection::count() const
{
    return (this->memory - this->active) / this->info.size;
}

nox::ecs::ComponentHandle<nox::ecs::Component>
nox::ecs::ComponentCollection::getComponent(const EntityId& id)
{
    ComponentHandle<Component> handle(id, 
                                      this->find(this->active,
                                                 this->memory,
                                                 id), 
                                      this->gen,
                                      this); 
    return handle;
}

std::size_t
nox::ecs::ComponentCollection::getGeneration() const
{
    return this->gen;
}

const nox::ecs::TypeIdentifier&
nox::ecs::ComponentCollection::getTypeIdentifier() const
{
    return this->info.typeIdentifier;
}

nox::ecs::Component*
nox::ecs::ComponentCollection::cast(Byte* entity) const
{
    return reinterpret_cast<Component*>(entity);
}

nox::ecs::Component*
nox::ecs::ComponentCollection::find(Byte* first, 
                                    Byte* last,
                                    const EntityId& id) const
{
    while (first != last)
    {
        auto component = this->cast(first);
        if (component->id == id)
        {
            return component;
        }
        first += this->info.size;
    }

    return nullptr;
}

std::size_t
nox::ecs::ComponentCollection::size() const
{
    return std::size_t(this->memory - this->active);
}

std::size_t
nox::ecs::ComponentCollection::capacity() const
{
    return std::size_t(this->cap - this->active);
}

void
nox::ecs::ComponentCollection::reallocate()
{
    const auto newCap = this->capacity() * GROWTH_FACTOR;

    // + this->info.size for swapArea
    const auto newFirst = static_cast<Byte*>(std::malloc(newCap + this->info.size));

    auto newItr = newFirst;
    auto begin = this->active;
    const auto end = this->memory;
    
    while (begin != end)
    {
        auto newComp = this->cast(newItr);
        auto oldComp = this->cast(begin);

        this->info.moveConstruct(newComp, oldComp);

        newItr += this->info.size;
        begin += this->info.size;
    }

    this->destroyRange(active, memory);

    this->inactive = newFirst + std::distance(this->active, this->inactive);
    this->hibernating = newFirst + std::distance(this->active, this->hibernating);
    this->memory = newFirst + std::distance(this->active, this->memory);
    this->cap = newFirst + newCap;

    std::free(this->active);

    this->active = newFirst;
    this->gen++;
}

void
nox::ecs::ComponentCollection::destroyRange(Byte* begin,
                                            Byte* end)
{
    while (begin != end)
    {
        auto ptr = this->cast(begin);
        this->info.destruct(ptr);
        begin += this->info.size;
    }
}

void
nox::ecs::ComponentCollection::swap(Component* lhs,
                                    Component* rhs)
{
    if (lhs != rhs)
    {
        auto swapArea = this->cast(this->cap);
        this->info.moveConstruct(swapArea, rhs);
        this->info.moveAssign(rhs, lhs);
        this->info.moveAssign(lhs, swapArea);   
        this->info.destruct(swapArea);
        this->gen++;
    }
}
