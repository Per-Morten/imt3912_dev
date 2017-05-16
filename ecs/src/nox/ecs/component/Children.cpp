#include <nox/ecs/component/Children.h>
#include <algorithm>

nox::ecs::Children::Children(Children&& source)
    : nox::ecs::Component(source.id, source.entityManager)
    , children(std::move(source.children))
{

}

nox::ecs::Children&
nox::ecs::Children::operator=(Children&& source)
{
    if (this != &source)
    {
        nox::ecs::Component::operator=(std::move(source)); 
        std::swap(this->children, source.children);
    }
    return *this;
}

void
nox::ecs::Children::addChild(const EntityId& childId)
{
    auto child = std::find(std::cbegin(children),
                           std::cend(children),
                           childId);
    if (child == std::cend(children))
    {
        this->children.push_back(childId);
    }
}

void
nox::ecs::Children::removeChild(const EntityId& childId)
{
    this->children.erase(std::remove(std::begin(this->children),
                                     std::end(this->children),
                                     childId),
                         std::end(this->children));
}

bool
nox::ecs::Children::hasChild(const EntityId& childId)
{
    return std::any_of(std::cbegin(this->children),
                       std::cend(this->children),
                       [childId](const auto& item) { return item == childId; });
}

nox::ecs::EntityId
nox::ecs::Children::operator[](std::size_t index) const
{
    return this->children[index];
}

std::size_t
nox::ecs::Children::size() const
{
    return this->children.size();
}

bool
nox::ecs::Children::empty() const
{
    return this->children.empty();
}
