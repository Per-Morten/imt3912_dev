#include <events/dummy_event.h>

#include <memory>


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
sendDummyEvent(nox::logic::actor::Actor* actor,
               nox::logic::IContext* logicContext,
               const std::string& senderName)
{
    //If the N is dividable with macro, call send message function
    //Otherwise call empty function
    sendGlobalDummyEvent<N % SEND_GLOBAL_EVENT_PER_N_COMPONMENT>(actor, 
                                                                 logicContext,
                                                                 senderName);

    //Same as above, but uses the local broadcast rather than the global one
    sendLocalDummyEvent<N % SEND_LOCAL_EVENT_PER_N_COMPONMENT>(actor, 
                                                               senderName);
}

template<std::size_t N>
void
sendLocalDummyEvent(nox::logic::actor::Actor* /*actor*/,
                    const std::string& /*senderName*/)
{
}

template<>
void
sendLocalDummyEvent<0>(nox::logic::actor::Actor* actor,
                       const std::string& senderName)
{
    auto dummyEvent = std::make_shared<DummyEvent>(actor,
                                                   senderName);
    actor->broadCastComponentEvent(dummyEvent);
}


template<std::size_t N>
void
sendGlobalDummyEvent(nox::logic::actor::Actor* /*actor*/,
                     nox::logic::IContext* /*logicContext*/,
                     const std::string& /*senderName*/)
{
}

template<>
void
sendGlobalDummyEvent<0>(nox::logic::actor::Actor* actor,
                        nox::logic::IContext* logicContext,
                        const std::string& senderName)
{
    auto dummyEvent = std::make_shared<DummyEvent>(actor,
                                                   senderName);
    logicContext->getEventBroadcaster()->queueEvent(dummyEvent);
}
