#include <nox/ecs/Entity.h>
#include <nox/util/nox_assert.h>

nox::ecs::Entity::Entity(const EntityId& id,
                         EntityManager* entityManager)
    : id(id)
    , entityManager(entityManager)
{
    NOX_ASSERT(entityManager, "EntityManager cannot be null");
}

void
nox::ecs::Entity::awake()
{
    this->entityManager->awakeEntity(this->id);
}

void
nox::ecs::Entity::activate()
{
    this->entityManager->activateEntity(this->id);
}

void
nox::ecs::Entity::deactivate()
{
    this->entityManager->deactivateEntity(this->id);
}

void
nox::ecs::Entity::hibernate()
{
    this->entityManager->hibernateEntity(this->id);
}

void
nox::ecs::Entity::remove()
{
    this->entityManager->removeEntity(this->id);
}

void
nox::ecs::Entity::awakeComponent(const TypeIdentifier& identifier)
{
    this->entityManager->awakeComponent(this->id, identifier);
}

void
nox::ecs::Entity::activateComponent(const TypeIdentifier& identifier)
{
    this->entityManager->activateComponent(this->id, identifier);
}

void
nox::ecs::Entity::deactivateComponent(const TypeIdentifier& identifier)
{
    this->entityManager->deactivateComponent(this->id, identifier);
}

void
nox::ecs::Entity::hibernateComponent(const TypeIdentifier& identifier)
{
    this->entityManager->hibernateComponent(this->id, identifier);
}

void
nox::ecs::Entity::assignComponent(const TypeIdentifier& identifier)
{
    this->entityManager->assignComponent(this->id, identifier);
}

void
nox::ecs::Entity::removeComponent(const TypeIdentifier& identifier)
{
    this->entityManager->removeComponent(this->id, identifier);
}

nox::ecs::ComponentHandle<nox::ecs::Component>
nox::ecs::Entity::getComponent(const TypeIdentifier& identifier)
{
    return this->entityManager->getComponent(this->id, identifier);
}

nox::ecs::EntityId
nox::ecs::Entity::getId() const
{
    return this->id;
}
