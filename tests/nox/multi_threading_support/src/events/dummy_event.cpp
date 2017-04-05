#include "dummy_event.h"

const DummyEvent::IdType DummyEvent::ID = "dummy.event";

DummyEvent::DummyEvent(nox::logic::actor::Actor* actor, 
                       const std::string& name)
    : Event(DummyEvent::ID, actor)
    , senderName(name)
    , receiverId(rand() % TRIVIAL_COMPONENT_COUNT)
{
}
