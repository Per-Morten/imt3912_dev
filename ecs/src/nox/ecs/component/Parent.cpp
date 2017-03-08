#include <nox/ecs/component/Parent.h>

nox::ecs::Parent::Parent(Parent&& source)
    : nox::ecs::Component(source.id)
    , parentId(std::move(source.parentId))
{

}

nox::ecs::Parent&
nox::ecs::Parent::operator=(Parent&& source)
{
    if (this != &source)
    {
        std::swap(this->id, source.id);
        std::swap(this->parentId, source.parentId);
    }
    return *this;
}
