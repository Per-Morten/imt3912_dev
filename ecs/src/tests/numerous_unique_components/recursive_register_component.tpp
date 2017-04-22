
template<std::size_t N>
void
registerTrivialComponent(nox::ecs::EntityManager& manager)
{
    auto info = nox::ecs::createMetaInformation<components::TrivialComponent<N>>(N + globals::first_unreserved_id);
    info.receiveEntityEventAccess = nox::ecs::DataAccess::INDEPENDENT;
    info.receiveLogicEventAccess = nox::ecs::DataAccess::INDEPENDENT;
    info.updateAccess = nox::ecs::DataAccess::INDEPENDENT;
    manager.registerComponent(info);
    registerTrivialComponent<N-1>(manager);
}

template<>
void
registerTrivialComponent<0>(nox::ecs::EntityManager& manager)
{
    auto info = nox::ecs::createMetaInformation<components::TrivialComponent<0>>(globals::first_unreserved_id);
    info.receiveEntityEventAccess = nox::ecs::DataAccess::INDEPENDENT;
    info.receiveLogicEventAccess = nox::ecs::DataAccess::INDEPENDENT;
    info.updateAccess = nox::ecs::DataAccess::INDEPENDENT;
    manager.registerComponent(info);
}
