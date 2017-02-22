template<class T>
nox::ecs::MetaInformation
nox::ecs::createMetaInformation(const TypeIdentifier& typeIdentifier)
{
    return createMetaInformation<T>(typeIdentifier, {});
}

template<class T>
nox::ecs::MetaInformation
nox::ecs::createMetaInformation(const TypeIdentifier& typeIdentifier, 
                                   const std::vector<nox::event::Event::IdType>& interestingLogicEvents)
{
    MetaInformation metaInformation(typeIdentifier, sizeof(T));
    
    const auto construct = [](Component* component, const EntityId& id)
    { 
        new(component)T(id);
    };

    const auto destruct = [](Component* component)
    {
        static_cast<T*>(component)->~T();
    };

    const auto initialize = [](Component* component, const Json::Value& value)
    {
        static_cast<T*>(component)->initialize(value);
    };

    const auto awake = [](Component* component)
    {
        static_cast<T*>(component)->awake();
    };

    const auto activate = [](Component* component)
    {
        static_cast<T*>(component)->activate();
    };

    const auto deactivate = [](Component* component)
    {
        static_cast<T*>(component)->deactivate();   
    };

    const auto hibernate = [](Component* component)
    {
        static_cast<T*>(component)->hibernate();   
    };

    const auto update = [](Component* first, Component* last,
                           const nox::Duration& deltaTime)
    {
        auto begin = static_cast<T*>(first);
        auto end = static_cast<T*>(last);

        while (begin != end)
        {
            begin->update(deltaTime);
            ++begin;
        }
    };


    const auto receiveLogicEvent = [](Component* first, Component* last,
                                      const std::shared_ptr<nox::event::Event>& event)
    {
        auto begin = static_cast<T*>(first);
        auto end = static_cast<T*>(last);

        while (begin != end)
        {
            begin->receiveLogicEvent(event);
            ++begin;
        }
    };

    const auto receiveComponentEvent = [](Component* first, Component* last,
                                          const nox::ecs::Event& event)
    {
        auto begin = static_cast<T*>(first);
        auto end = static_cast<T*>(last);

        while (begin != end)
        {
            begin->receiveComponentEvent(event);
            ++begin;
        }
    };

    metaInformation.construct = construct;
    metaInformation.destruct = destruct;
    metaInformation.initialize = initialize;
    metaInformation.awake = awake;
    metaInformation.activate = activate;
    metaInformation.deactivate = deactivate;
    metaInformation.hibernate = hibernate;
    metaInformation.update = update;
    metaInformation.receiveLogicEvent = receiveLogicEvent;
    metaInformation.receiveComponentEvent = receiveComponentEvent;
    metaInformation.interestingLogicEvents = interestingLogicEvents;
}
