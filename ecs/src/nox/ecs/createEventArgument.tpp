template<class T>
void
nox::ecs::createEventArgument(Event& event,
                              const T& argument,
                              const TypeIdentifier& identifier)
{
    static_assert(sizeof(T) + sizeof(Event::Argument) <= Event::ArgumentAllocator::MAX_SIZE,
                   "sizeof T + sizeof Event::Argument is larger than Event::ArgumentAllocator::MAX_SIZE");
    auto memory = static_cast<nox::memory::Byte*>(event.getAllocator().allocate(sizeof(T) + sizeof(Event::Argument)));

    auto payloadStart = memory + sizeof(Event::Argument);
    new(payloadStart)T(argument);

    auto destructor = [](nox::memory::Byte* payload)
    {
        reinterpret_cast<T*>(payload)->~T();
    };

    auto arg = new(memory) Event::Argument(identifier, payloadStart, destructor);

    event.addArgument(arg);
}
