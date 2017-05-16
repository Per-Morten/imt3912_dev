#pragma once

#include <nox/logic/actor/Actor.h>
#include <nox/logic/actor/event/Event.h>


class DummyEvent
    : public nox::logic::actor::Event
{
public:
    static const IdType ID;

    DummyEvent(nox::logic::actor::Actor* actor, 
               const std::string& name);

public:
    std::string senderName;
    std::size_t receiverId;

};
