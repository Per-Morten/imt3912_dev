#include <events/dummy_event.h>

#include <memory>

#ifndef SEND_EVENT_PER_N_COMPONMENT
    #error must define SEND_EVENT_PER_N_COMPONMENT!
#endif

#if SEND_EVENT_PER_N_COMPONMENT == 0
    #error SEND_EVENT_PER_N_COMPONMENT cannot be equals to 0!
#endif

template<std::size_t N>
void
sendDummyEvent(nox::logic::actor::Actor* actor,
               nox::logic::IContext* logicContext,
               const std::string& senderName)
{
    //If the N is dividable with 10, call send message function
    //Otherwise call empty function
    sendDummyEventHelper<N % SEND_EVENT_PER_N_COMPONMENT>(actor, 
                                                          logicContext,
                                                          senderName);
}

template<std::size_t N>
void
sendDummyEventHelper(nox::logic::actor::Actor* actor,
                     nox::logic::IContext* logicContext,
                     const std::string& senderName)
{
}

template<>
void
sendDummyEventHelper<0>(nox::logic::actor::Actor* actor,
                        nox::logic::IContext* logicContext,
                        const std::string& senderName)
{
    auto dummyEvent = std::make_shared<DummyEvent>(actor,
                                                   senderName);
    logicContext->getEventBroadcaster()->queueEvent(dummyEvent);
}

