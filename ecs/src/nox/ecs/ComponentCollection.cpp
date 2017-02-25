#include <nox/ecs/ComponentCollection.h>

#include <cstdlib>

nox::ecs::ComponentCollection::ComponentCollection(const MetaInformation& info)
    : info(info)
    , gen(0)
    , first(static_cast<Byte*>(std::malloc(GROWTH_FACTOR * info.size)))
    , last(first)
    , cap(first + (GROWTH_FACTOR * info.size))
{
}

nox::ecs::ComponentCollection::ComponentCollection(ComponentCollection&& source)
    : info(std::move(source.info))
    , gen(std::move(source.gen))
    , first(std::move(source.first))
    , last(std::move(source.last))
    , cap(std::move(source.cap))
{
    source.first = nullptr;
    source.last = nullptr;
    source.cap = nullptr;
    source.gen = 0;
}

nox::ecs::ComponentCollection&
nox::ecs::ComponentCollection::operator=(ComponentCollection&& source)
{
    if (this != &source)
    {
        this->destroyRange(first, last);
        std::free(first);
        this->info = std::move(source.info);
        this->first = std::move(source.first);
        this->last = std::move(source.last);
        this->cap = std::move(source.cap);
        this->gen = std::move(source.gen);

        source.first = nullptr;
        source.last = nullptr;
        source.cap = nullptr;
        source.gen = 0;
    }

    return *this;
}

nox::ecs::ComponentCollection::~ComponentCollection()
{
    auto begin = this->first;
    const auto end = this->last;

    while (begin != end)
    {
        auto ptr = this->cast(begin);
        this->info.destruct(ptr);
        begin += this->info.size;
    }

    std::free(this->first);
    this->first = nullptr;
    this->last = nullptr;
    this->cap = nullptr;
}

void
nox::ecs::ComponentCollection::create(const EntityId& id)
{
    if (this->size() >= this->capacity())
    {
        this->reallocate();
    }

    this->info.construct(this->cast(this->last), id);
    this->last += this->info.size;
}

void
nox::ecs::ComponentCollection::initialize(const EntityId& id,
                                          const Json::Value& value)
{
    auto component = this->find(id);
    if (component && this->info.initialize)
    {
        this->info.initialize(component, value);
    }
}

void
nox::ecs::ComponentCollection::awake(const EntityId& id)
{
    auto component = this->find(id);
    if (component && this->info.awake)
    {
        this->info.awake(component);
    }
}

void
nox::ecs::ComponentCollection::activate(const EntityId& id)
{
    auto component = this->find(id);
    if (component && this->info.activate)
    {
        this->info.activate(component);
    }
}

void
nox::ecs::ComponentCollection::deactivate(const EntityId& id)
{
    auto component = this->find(id);
    if (component && this->info.deactivate)
    {
        this->info.deactivate(component);
    }
}

void
nox::ecs::ComponentCollection::hibernate(const EntityId& id)
{
    auto component = this->find(id);
    if (component && this->info.hibernate)
    {
        this->info.hibernate(component);
    }
}

void
nox::ecs::ComponentCollection::remove(const EntityId& id)
{
    auto component = this->find(id);
    
    if (component)
    {
        auto nextToLast = this->last - this->info.size;
        auto ptr = this->cast(nextToLast);

        this->info.moveAssign(component, ptr);

        this->last -= this->info.size;
        this->info.destruct(this->cast(last));
    }

    this->gen++; 
}

void
nox::ecs::ComponentCollection::update(const nox::Duration& duration)
{
    auto begin = this->cast(this->first);
    auto end = this->cast(this->last);

    this->info.update(begin, end, duration);
}

std::size_t
nox::ecs::ComponentCollection::count() const
{
    return this->size() / this->info.size;
}

nox::ecs::ComponentHandle<nox::ecs::Component>
nox::ecs::ComponentCollection::getComponent(const EntityId& id)
{
    ComponentHandle<Component> handle(id, 
                                      this->find(id), 
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
nox::ecs::ComponentCollection::find(const EntityId& id) const
{
    auto begin = this->first;
    const auto end = this->last;
    
    while (begin != end)
    {
        auto component = this->cast(begin);
        if (component->id == id)
        {
            return component;
        }
        begin += this->info.size;
    }

    return nullptr;
}

std::size_t
nox::ecs::ComponentCollection::size() const
{
    return std::size_t(this->last - this->first);
}

std::size_t
nox::ecs::ComponentCollection::capacity() const
{
    return std::size_t(this->cap - this->first);
}

void
nox::ecs::ComponentCollection::reallocate()
{
    const auto oldSize = this->size();
    const auto newCap = this->capacity() * GROWTH_FACTOR;
    const auto newFirst = static_cast<Byte*>(std::malloc(newCap));

    auto newItr = newFirst;
    auto begin = this->first;
    const auto end = this->last;
    
    while (begin != end)
    {
        auto newComp = this->cast(newItr);
        auto oldComp = this->cast(begin);

        this->info.moveConstruct(newComp, oldComp);

        newItr += this->info.size;
        begin += this->info.size;
    }

    this->destroyRange(first, last);

    std::free(this->first);

    this->first = newFirst;
    this->last = newFirst + oldSize;
    this->cap = newFirst + newCap;

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
