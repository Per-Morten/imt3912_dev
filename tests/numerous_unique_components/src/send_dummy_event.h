#pragma once
#include <nox/logic/IContext.h>
#include <nox/logic/actor/Actor.h>

//Meta template functions that sends events if N is dividable by 10


//Initial function
template<std::size_t N>
void 
sendDummyEvent(nox::logic::actor::Actor* actor,
               nox::logic::IContext* logicContext,
               const std::string& senderName);

//Empty function
template<std::size_t N>
void
sendDummyEventHelper(nox::logic::actor::Actor* actor,
                     nox::logic::IContext* logicContext,
                     const std::string& senderName);

//Send message function
template<>
void
sendDummyEventHelper<0>(nox::logic::actor::Actor* actor,
                        nox::logic::IContext* logicContext,
                        const std::string& senderName);

#include "send_dummy_event.tpp"
