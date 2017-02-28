
template<std::size_t N>
void
registerTrivialComponent(nox::logic::world::Manager* world)
{
    world->registerActorComponent<components::TrivialComponent<N>>();
    registerTrivialComponent<N-1>(world);
}

template<>
void
registerTrivialComponent<0>(nox::logic::world::Manager* world)
{
    world->registerActorComponent<components::TrivialComponent<0>>();
}
