#include <string>

#include <nox/event/IBroadcaster.h>

#include <send_dummy_event.h>

#define PT_DONT_OPTIMIZE asm("");

template<std::size_t duration>
const nox::logic::actor::Component::IdType components::TrivialComponent<duration>::NAME = std::string("TrivialComponent" + std::to_string(duration));

template<std::size_t duration>
components::TrivialComponent<duration>::TrivialComponent()
    : listener(this->NAME)
{    
}

template<std::size_t duration>
const nox::logic::actor::Component::IdType& 
components::TrivialComponent<duration>::getName() const
{
    return this->NAME;
}

template<std::size_t duration>
bool 
components::TrivialComponent<duration>::initialize(const Json::Value& /*componentJsonObject*/)
{
    return true;
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onCreate()
{
    this->listener.setup(this, this->getLogicContext()->getEventBroadcaster());
    this->listener.addEventTypeToListenFor(DummyEvent::ID);
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onActivate()
{
    this->listener.startListening();
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onDeactivate()
{
    this->listener.stopListening();
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onUpdate(const nox::Duration& deltaTime)
{
    const auto clockStart = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds currentDuration(0);

    sendDummyEvent<duration>(this->getOwner(), 
                                 this->getLogicContext(),
                                 this->NAME);

    while (currentDuration < this->sleepDuration)
    {
        const auto now = std::chrono::high_resolution_clock::now();
        currentDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - clockStart);
    }
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onEvent(const std::shared_ptr<nox::event::Event>& event)
{
    if (event->getType() == DummyEvent::ID)
    {
        auto dummyEvent = static_cast<DummyEvent*>(event.get());

        if (dummyEvent->receiverId == duration)
        {
            getLog().info().format("Message sent from actor \"%s\" to actor \"%s\"", dummyEvent->senderName, NAME);
        }
    }
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::serialize(Json::Value& /*componentObject*/)
{
}
