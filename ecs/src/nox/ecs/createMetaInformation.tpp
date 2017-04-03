#include <type_traits>
#include <nox/ecs/Component.h>

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
    static_assert(std::is_base_of<nox::ecs::Component, T>::value, "Type T must inherit from nox::ecs::Component");
    MetaInformation metaInformation(typeIdentifier, sizeof(T));
    
    const auto construct = [](Component* component, const EntityId& id, EntityManager* manager)
    { 
        new(component)T(id, manager);
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

    const auto receiveEntityEvent = [](Component* first,
                                       Component* last,
                                       const nox::ecs::Event& event)
    {
        auto begin = static_cast<T*>(first);
        auto end = static_cast<T*>(last);

        while (begin != end)
        {
            begin->receiveEntityEvent(event);
            ++begin;
        }
    };

    const auto moveConstruct = [](Component* dest, Component* src)
    {
        auto srcComp = static_cast<T*>(src);

        new(dest)T(std::move(*srcComp));
    };

    const auto moveAssign = [](Component* dest, Component* src)
    {
        auto destComp = static_cast<T*>(dest);
        auto srcComp = static_cast<T*>(src);

        *destComp = std::move(*srcComp); 
    };

    metaInformation.construct = construct;
    metaInformation.destruct = destruct;
    metaInformation.initialize = initialize;
    metaInformation.awake = awake;
    metaInformation.activate = activate;
    metaInformation.deactivate = deactivate;
    metaInformation.hibernate = hibernate;
    metaInformation.update = update;
    metaInformation.moveConstruct = moveConstruct;
    metaInformation.moveAssign = moveAssign;
    metaInformation.receiveLogicEvent = receiveLogicEvent;
    metaInformation.receiveEntityEvent = receiveEntityEvent;
    metaInformation.interestingLogicEvents = interestingLogicEvents;

    #ifdef NOX_ECS_COMPONENT_UNIQUE_PTR_VIRTUAL
    const auto create = [](const nox::ecs::EntityId& id, nox::ecs::EntityManager* manager) -> std::unique_ptr<Component>
    {
        return std::make_unique<T>(id, manager);
    };

    const auto moveCreate = [](Component& comp) -> std::unique_ptr<Component>
    {
        auto& tmp = static_cast<T&>(comp);
        return std::make_unique<T>(std::move(tmp));
    };

    metaInformation.virtualCreate = create;
    metaInformation.virtualMoveCreate = moveCreate;
    #endif 

    return metaInformation;
}
