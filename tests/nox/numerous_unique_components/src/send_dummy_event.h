#pragma once
#include <nox/logic/IContext.h>
#include <nox/logic/actor/Actor.h>

//Meta template functions that sends events


//Initial function
template<std::size_t N>
void 
sendDummyEvent(nox::logic::actor::Actor* actor,
               nox::logic::IContext* logicContext,
               const std::string& senderName);

//Empty local function
template<std::size_t N>
void
sendLocalDummyEvent(nox::logic::actor::Actor* actor,
                    const std::string& senderName);

//Send local message function
template<>
void
sendLocalDummyEvent<0>(nox::logic::actor::Actor* actor,
                       const std::string& senderName);


//Empty function
template<std::size_t N>
void
sendGlobalDummyEvent(nox::logic::actor::Actor* actor,
                     nox::logic::IContext* logicContext,
                     const std::string& senderName);

//Send message function
template<>
void
sendGlobalDummyEvent<0>(nox::logic::actor::Actor* actor,
                        nox::logic::IContext* logicContext,
                        const std::string& senderName);


#include "send_dummy_event.tpp"
