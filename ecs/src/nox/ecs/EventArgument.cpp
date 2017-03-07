#include <nox/ecs/EventArgument.h>

nox::ecs::EventArgument::EventArgument(Byte* payload,
                                       const TypeIdentifier& identifier,
                                       Destructor destructor)
    : type(identifier)
    , payload(payload)
    , destructor(destructor)
{

}

void*
nox::ecs::EventArgument::value() const
{
    return this->payload;
}

const nox::ecs::TypeIdentifier&
nox::ecs::EventArgument::getType() const
{
    return this->type;
}

nox::ecs::EventArgument::Destructor
nox::ecs::EventArgument::getDestructor() const
{
    return this->destructor;
}
