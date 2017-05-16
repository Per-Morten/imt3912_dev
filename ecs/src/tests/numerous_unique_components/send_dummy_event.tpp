#include <memory>
#include <nox/util/pms_debug.h>

#include <globals.h>
#include <nox/ecs/createEventArgument.h>

#ifndef SEND_LOCAL_EVENT_PER_N_COMPONMENT
    #error must define SEND_LOCAL_EVENT_PER_N_COMPONMENT!
#endif

#if SEND_LOCAL_EVENT_PER_N_COMPONMENT == 0
    #error SEND_LOCAL_EVENT_PER_N_COMPONMENT cannot be equals to 0!
#endif


#ifndef SEND_GLOBAL_EVENT_PER_N_COMPONMENT
    #error must define SEND_GLOBAL_EVENT_PER_N_COMPONMENT!
#endif

#if SEND_GLOBAL_EVENT_PER_N_COMPONMENT == 0
    #error SEND_GLOBAL_EVENT_PER_N_COMPONMENT cannot be equals to 0!
#endif


template<std::size_t N>
void
sendDummyEvent(nox::ecs::EntityId senderId,
               nox::ecs::EntityManager* manager)
{
    //If the N is dividable with macro, call send message function
    //Otherwise call empty function
    sendDummyEventHelper<N % SEND_LOCAL_EVENT_PER_N_COMPONMENT>(senderId,
                                                                 manager);

    sendGlobalDummyEventHelper<N % SEND_GLOBAL_EVENT_PER_N_COMPONMENT>(senderId, 
                                                                       manager);

}

template<std::size_t N>
void
sendDummyEventHelper(nox::ecs::EntityId /*senderId*/,
                     nox::ecs::EntityManager* /*manager*/)
{
}

template<>
void
sendDummyEventHelper<0>(nox::ecs::EntityId senderId,
                        nox::ecs::EntityManager* manager)
{
    auto event = std::move(manager->createEntityEvent(globals::dummy_event,
                                                      senderId,
                                                      senderId));

    nox::ecs::createEventArgument(event, senderId, globals::dummy_event_sender_arg);
    nox::ecs::createEventArgument(event, rand() % TRIVIAL_COMPONENT_COUNT, globals::dummy_event_receiver_arg);
    manager->sendEntityEvent(std::move(event));
}

template<std::size_t N>
void
sendGlobalDummyEventHelper(nox::ecs::EntityId /*senderId*/,
                     nox::ecs::EntityManager* /*manager*/)
{
}

template<>
void
sendGlobalDummyEventHelper<0>(nox::ecs::EntityId senderId,
                        nox::ecs::EntityManager* manager)
{
    auto event = std::move(manager->createEntityEvent(globals::dummy_event,
                                                      senderId));

    nox::ecs::createEventArgument(event, senderId, globals::dummy_event_sender_arg);
    nox::ecs::createEventArgument(event, rand() % TRIVIAL_COMPONENT_COUNT, globals::dummy_event_receiver_arg);
    manager->sendEntityEvent(std::move(event));
}
