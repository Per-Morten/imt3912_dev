template<class T>
nox::ecs::Event::Argument
nox::ecs::createEventArgument(const T& argument,
                              const TypeIdentifier& identifier)
{
    auto memory = static_cast<Event::Argument::Byte*>(std::malloc(sizeof(T)));
    new(memory)T(argument);

    auto destructor = [](Event::Argument::Byte* payload)
    {
        reinterpret_cast<T*>(payload)->~T();
        std::free(payload);
    };

    return {memory, identifier, destructor};
}
