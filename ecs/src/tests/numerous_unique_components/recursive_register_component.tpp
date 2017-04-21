
template<std::size_t N>
void
registerTrivialComponent(nox::ecs::EntityManager& manager)
{
    auto info = nox::ecs::createMetaInformation<components::TrivialComponent<N>>(N + 1000);
    manager.registerComponent(info);
    registerTrivialComponent<N-1>(manager);
}

template<>
void
registerTrivialComponent<0>(nox::ecs::EntityManager& manager)
{
    auto info = nox::ecs::createMetaInformation<components::TrivialComponent<0>>(0 + 1000);
    manager.registerComponent(info);
}
