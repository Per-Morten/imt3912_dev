
template<std::size_t N>
void
registerTrivialComponent(nox::ecs::EntityManager& manager)
{
    auto info = nox::ecs::createMetaInformation<components::TrivialComponent<N>>(N + globals::first_unreserved_id);

    const nox::ecs::DataAccess entityEvent = static_cast<nox::ecs::DataAccess>((N + 0) % 4);
    const nox::ecs::DataAccess logicEvent = static_cast<nox::ecs::DataAccess>((N + 1) % 4);
    const nox::ecs::DataAccess update = static_cast<nox::ecs::DataAccess>((N + 2) % 4);


    if (N % 5 == 0)
    {
        info.update = nullptr;
    }
    
    if (N % 3 == 0)
    {
        info.receiveEntityEvent = nullptr;
    }

    if (entityEvent == nox::ecs::DataAccess::INDEPENDENT)
    {
        info.receiveEntityEventAccess = nox::ecs::DataAccess::INDEPENDENT;
    }
    else if (entityEvent == nox::ecs::DataAccess::READ_ONLY)  
    {
        info.receiveEntityEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveEntityEventDependencies.push_back((N + 1) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
    }
    else if (entityEvent == nox::ecs::DataAccess::READ_WRITE)  
    {
        info.receiveEntityEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveEntityEventDependencies.push_back((N + 1) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
        info.receiveEntityEventDependencies.push_back((N + 2) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
    }

    if (logicEvent == nox::ecs::DataAccess::INDEPENDENT)
    {
        info.receiveLogicEventAccess = nox::ecs::DataAccess::INDEPENDENT;
    }
    else if (logicEvent == nox::ecs::DataAccess::READ_ONLY)  
    {
        info.receiveLogicEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveLogicEventDependencies.push_back((N + 1) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
    }
    else if (logicEvent == nox::ecs::DataAccess::READ_WRITE)  
    {
        info.receiveLogicEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveLogicEventDependencies.push_back((N + 1) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
        info.receiveLogicEventDependencies.push_back((N + 2) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
    }

    if (update == nox::ecs::DataAccess::INDEPENDENT)
    {
        info.updateAccess = nox::ecs::DataAccess::INDEPENDENT;
    }
    else if (update == nox::ecs::DataAccess::READ_ONLY)  
    {
        info.updateAccess = nox::ecs::DataAccess::READ_ONLY;
        info.updateDependencies.push_back((N + 1) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
    }
    else if (update == nox::ecs::DataAccess::READ_WRITE)  
    {
        info.updateAccess = nox::ecs::DataAccess::READ_ONLY;
        info.updateDependencies.push_back((N + 1) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
        info.updateDependencies.push_back((N + 2) % TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id);
    }

    manager.registerComponent(info);
    registerTrivialComponent<N-1>(manager);
}

template<>
void
registerTrivialComponent<0>(nox::ecs::EntityManager& manager)
{
    auto info = nox::ecs::createMetaInformation<components::TrivialComponent<0>>(globals::first_unreserved_id);

    const nox::ecs::DataAccess entityEvent = static_cast<nox::ecs::DataAccess>(0);
    const nox::ecs::DataAccess logicEvent = static_cast<nox::ecs::DataAccess>(1);
    const nox::ecs::DataAccess update = static_cast<nox::ecs::DataAccess>(2);

    info.update = nullptr;
    info.receiveEntityEvent = nullptr;

    if (entityEvent == nox::ecs::DataAccess::INDEPENDENT)
    {
        info.receiveEntityEventAccess = nox::ecs::DataAccess::INDEPENDENT;   
    }
    else if (entityEvent == nox::ecs::DataAccess::READ_ONLY)  
    {
        info.receiveEntityEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveEntityEventDependencies.push_back(1 + globals::first_unreserved_id);
    }
    else if (entityEvent == nox::ecs::DataAccess::READ_WRITE)  
    {
        info.receiveEntityEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveEntityEventDependencies.push_back(1 + globals::first_unreserved_id);
        info.receiveEntityEventDependencies.push_back(2 + globals::first_unreserved_id);
    }

    if (logicEvent == nox::ecs::DataAccess::INDEPENDENT)
    {
        info.receiveLogicEventAccess = nox::ecs::DataAccess::INDEPENDENT;   
    }
    else if (logicEvent == nox::ecs::DataAccess::READ_ONLY)  
    {
        info.receiveLogicEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveLogicEventDependencies.push_back(1 + globals::first_unreserved_id);
    }
    else if (logicEvent == nox::ecs::DataAccess::READ_WRITE)  
    {
        info.receiveLogicEventAccess = nox::ecs::DataAccess::READ_ONLY;
        info.receiveLogicEventDependencies.push_back(1 + globals::first_unreserved_id);
        info.receiveLogicEventDependencies.push_back(2 + globals::first_unreserved_id);
    }

    if (update == nox::ecs::DataAccess::INDEPENDENT)
    {
        info.updateAccess = nox::ecs::DataAccess::INDEPENDENT;   
    }
    else if (update == nox::ecs::DataAccess::READ_ONLY)  
    {
        info.updateAccess = nox::ecs::DataAccess::READ_ONLY;
        info.updateDependencies.push_back(1 + globals::first_unreserved_id);
    }
    else if (update == nox::ecs::DataAccess::READ_WRITE)  
    {
        info.updateAccess = nox::ecs::DataAccess::READ_ONLY;
        info.updateDependencies.push_back(1 + globals::first_unreserved_id);
        info.updateDependencies.push_back(2 + globals::first_unreserved_id);
    }

    manager.registerComponent(info);
}
