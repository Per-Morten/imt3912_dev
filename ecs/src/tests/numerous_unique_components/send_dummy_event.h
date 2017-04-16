#pragma once
#include <nox/logic/IContext.h>
#include <nox/logic/actor/Actor.h>
#include <nox/ecs/EntityId.h>
#include <nox/ecs/EntityManager.h>

//Meta template functions that sends events


//Initial function
template<std::size_t N>
void 
sendDummyEvent(nox::ecs::EntityId senderId,
               nox::ecs::EntityManager* manager);

//Empty function
template<std::size_t N>
void
sendDummyEventHelper(nox::ecs::EntityId senderId,
                     nox::ecs::EntityManager* manager);

//Send message function
template<>
void
sendDummyEventHelper<0>(nox::ecs::EntityId senderId,
                        nox::ecs::EntityManager* manager);


#include "send_dummy_event.tpp"
