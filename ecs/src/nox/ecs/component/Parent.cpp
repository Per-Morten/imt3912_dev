#include <nox/ecs/component/Parent.h>

nox::ecs::Parent::Parent(Parent&& source)
    : nox::ecs::Component(source.id, source.entityManager)
    , parentId(std::move(source.parentId))
{

}

nox::ecs::Parent&
nox::ecs::Parent::operator=(Parent&& source)
{
    if (this != &source)
    {
        nox::ecs::Component::operator=(std::move(source));
        std::swap(this->parentId, source.parentId);
    }
    return *this;
}
