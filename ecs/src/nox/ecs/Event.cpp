#include <nox/ecs/Event.h>

#include <algorithm>

#include <nox/util/nox_assert.h>

nox::ecs::Event::Event(const EntityId& receiverId,
                       const TypeIdentifier& eventType)
    : receiver(receiverId) 
    , type(eventType)
{

}

nox::ecs::Event::~Event()
{
    for (auto& item : this->arguments)
    {
        auto destructor = item.arg.getDestructor();
        destructor(static_cast<EventArgument::Byte*>(item.arg.value()));
    }
}

void
nox::ecs::Event::addArgument(const TypeIdentifier& name,
                             EventArgument argument)
{
    NOX_ASSERT(std::none_of(std::cbegin(this->arguments),
                            std::cend(this->arguments),
                            [&name](const auto& item)
                            { return item.name == name; }), 
               "Event was given duplicate arguments!");

    this->arguments.push_back({ name, std::move(argument) });
}

bool
nox::ecs::Event::hasArgument(const TypeIdentifier& name) const
{
    return std::any_of(std::cbegin(this->arguments),
                       std::cend(this->arguments),
                       [&name](const auto& item)
                       { return item.name == name; });
}

const nox::ecs::EventArgument&
nox::ecs::Event::getArgument(const TypeIdentifier& name) const
{
    auto argument = std::find_if(std::cbegin(this->arguments),
                                 std::cend(this->arguments),
                                 [&name](const auto& item)
                                 { return item.name == name; });

    NOX_ASSERT(argument != std::cend(this->arguments), "Event has no argument with type identifier: %zu", name.getValue());

    return argument->arg;
}

const nox::ecs::TypeIdentifier&
nox::ecs::Event::getType() const
{
    return this->type;
}

const nox::ecs::EntityId&
nox::ecs::Event::getReceiver() const
{
    return this->receiver;
}
