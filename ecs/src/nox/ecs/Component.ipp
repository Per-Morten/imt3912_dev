#include <nox/util/pms_debug.h>

nox::ecs::Component::Component(const EntityId& id,
                               EntityManager* entityManager)
    : id(id)
    , entityManager(entityManager)
{
    // if (!entityManager)
    // {
    //     PMS_DEBUG("EntityManager is nullptr\n");
    // }
}

nox::ecs::Component::Component(Component&& other)
    : id(other.id)
    , entityManager(other.entityManager)
{
    // PMS_DEBUG("Move constructor\n");

    // if (!entityManager)
    // {
    //     PMS_DEBUG("EntityManager is nullptr\n");
    // }
}

nox::ecs::Component&
nox::ecs::Component::operator=(Component&& other)
{
    //PMS_DEBUG("Move operator\n");
    if (this != &other)
    {
        this->id = other.id;
        this->entityManager = other.entityManager;
    }
    // if (!other.entityManager)
    // {
    //     PMS_DEBUG("Other entityManager is nullptr\n");
    // }

    return *this;
}
