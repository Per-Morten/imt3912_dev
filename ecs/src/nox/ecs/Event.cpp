#include <nox/ecs/Event.h>

#include <algorithm>

#include <nox/util/nox_assert.h>

///////////
// Event //
/////////// 
const nox::ecs::EntityId nox::ecs::Event::BROADCAST = std::numeric_limits<EntityId>::max();

nox::ecs::Event::Event(const TypeIdentifier& eventType,
                       const EntityId& senderId,
                       const EntityId& receiverId)
    : receiverId(receiverId) 
    , senderId(senderId)
    , type(eventType)
{

}

nox::ecs::Event::~Event()
{
    for (auto& item : this->arguments)
    {
        auto destructor = item.arg.destructor;
        destructor(item.arg.payload);
    }
}

void
nox::ecs::Event::addArgument(const TypeIdentifier& identifier,
                             Argument argument)
{
    NOX_ASSERT(std::none_of(std::cbegin(this->arguments),
                            std::cend(this->arguments),
                            [&identifier](const auto& item)
                            { return item.identifier == identifier; }), 
               "Event was given duplicate arguments!");

    this->arguments.push_back({ identifier, std::move(argument) });
}

bool
nox::ecs::Event::hasArgument(const TypeIdentifier& identifier) const
{
    return std::any_of(std::cbegin(this->arguments),
                       std::cend(this->arguments),
                       [&identifier](const auto& item)
                       { return item.identifier == identifier; });
}


const nox::ecs::Event::Argument&
nox::ecs::Event::getArgument(const TypeIdentifier& identifier) const
{
    const auto argument = std::find_if(std::cbegin(this->arguments),
                                       std::cend(this->arguments),
                                       [&identifier](const auto& item)
                                       { return item.identifier == identifier; });
        
    NOX_ASSERT(argument != std::cend(this->arguments), "Event has no argument with type identifier: %zu", identifier.getValue());

    return argument->arg;
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

/////////////////////
// Event::Argument //
/////////////////////
nox::ecs::Event::Argument::Argument(Byte* payload,
                                    const TypeIdentifier& identifier,
                                    Destructor destructor)
    : type(identifier)
    , payload(payload)
    , destructor(destructor)
{

}

nox::ecs::Event::Argument::Argument(Event::Argument&& source)
    : type(source.type)
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
        this->type = source.type;

        source.destructor = nullptr;
        source.payload = nullptr;
    }
    return *this;
}

const void* const
nox::ecs::Event::Argument::value() const
{
    return this->payload;
}

const nox::ecs::TypeIdentifier&
nox::ecs::Event::Argument::getType() const
{
    return this->type;
}
