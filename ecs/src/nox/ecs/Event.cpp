#include <nox/ecs/Event.h>
///////////
// Event //
///////////

const nox::ecs::EntityId nox::ecs::Event::BROADCAST = std::numeric_limits<nox::ecs::EntityId>::max();

nox::ecs::Event::Event(ArgumentAllocator* allocator,
                       const TypeIdentifier& eventType,
                       const EntityId& senderId,
                       const EntityId& receiverId)
    : receiverId(receiverId) 
    , senderId(senderId)
    , type(eventType)
    , allocator(allocator)
{

}

nox::ecs::Event::Event(Event&& source)
    : receiverId(source.receiverId)
    , senderId(source.senderId)
    , type(source.type)
    , allocator(source.allocator)
    , first(source.first)
{
    source.first = nullptr;
}

nox::ecs::Event&
nox::ecs::Event::operator=(Event&& source)
{
    if (this != &source)
    {
        while (first)
        {
            auto next = first->next;
            first->destructor(first->payload);
            allocator->deallocate(first);
            first = next;
        }

        this->receiverId = source.receiverId;
        this->senderId = source.senderId;
        this->type = source.type;
        this->allocator = source.allocator;
        this->first = source.first;

        source.first = nullptr;
    }
    return *this;
}

nox::ecs::Event::~Event()
{
    while (first)
    {
        auto next = first->next;
        first->destructor(first->payload);
        allocator->deallocate(first);
        first = next;
    }
}

void
nox::ecs::Event::addArgument(Argument* argument)
{
    NOX_ASSERT(!this->isDuplicate(argument->getIdentifier()), "Event was given duplicate arguments!");

    if (!first)
    {
        first = argument;
        return;
    }

    auto before = first; 
    auto itr = first->next;
    while (itr)
    {
        before = itr;
        itr = itr->next;
    }
    before->next = argument;
}

bool
nox::ecs::Event::hasArgument(const TypeIdentifier& identifier) const
{
    auto itr = first;
    while (itr)
    {
        if (itr->getIdentifier() == identifier)
        {
            return true;
        }
        itr = itr->next;
    }
    return false;
}


const nox::ecs::Event::Argument&
nox::ecs::Event::getArgument(const TypeIdentifier& identifier) const
{
    auto itr = first;
    while (itr)
    {
        if (itr->getIdentifier() == identifier)
        {
            break;
        }
        itr = itr->next;
    }

    NOX_ASSERT(itr, "Event has no argument with type identifier: %zu", identifier.getValue());

    return *itr;
}

const nox::ecs::TypeIdentifier&
nox::ecs::Event::getType() const
{
    return this->type;
}

const nox::ecs::EntityId&
nox::ecs::Event::getSender() const
{
    return this->senderId;
}

const nox::ecs::EntityId&
nox::ecs::Event::getReceiver() const
{
    return this->receiverId;
}

nox::ecs::Event::ArgumentAllocator&
nox::ecs::Event::getAllocator()
{
    return *this->allocator;
}

bool 
nox::ecs::Event::isDuplicate(const TypeIdentifier& identifier) const
{
    auto itr = first;
    while (itr)
    {
        if (itr->getIdentifier() == identifier)
        {
            return true;
        }
        itr = itr->next;
    }
    return false;
}

/////////////////////
// Event::Argument //
/////////////////////
nox::ecs::Event::Argument::Argument(const TypeIdentifier& identifier,
                                    nox::memory::Byte* payload,
                                    Destructor destructor)
    : identifier(identifier)
    , payload(payload)
    , destructor(destructor)
{

}

nox::ecs::Event::Argument::Argument(Event::Argument&& source)
    : identifier(source.identifier)
    , payload(source.payload)
    , destructor(source.destructor)
{
    source.payload = nullptr;
    source.destructor = nullptr;
}

nox::ecs::Event::Argument&
nox::ecs::Event::Argument::operator=(Event::Argument&& source)
{
    if (this != &source)
    {
        this->destructor(payload);
        this->destructor = source.destructor;
        this->payload = source.payload;
        this->identifier = source.identifier;

        source.destructor = nullptr;
        source.payload = nullptr;
    }
    return *this;
}

const nox::ecs::TypeIdentifier&
nox::ecs::Event::Argument::getIdentifier() const
{
    return this->identifier;
}
