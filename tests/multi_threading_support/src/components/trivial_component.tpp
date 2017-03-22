#include <string>

#include <nox/event/IBroadcaster.h>

#include <cmd/parser.h>
#include <globals.h>
#include <send_dummy_event.h>


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
    auto durationMultiplier = cmd::g_cmdParser.getFloatArgument(cmd::constants::trivial_component_duration_multiplier_cmd,
                                                                cmd::constants::trivial_component_duration_multiplier_default);

    auto durationExponent = cmd::g_cmdParser.getFloatArgument(cmd::constants::trivial_component_duration_exponent_cmd,
                                                              cmd::constants::trivial_component_duration_exponent_default);

    std::size_t durationInNs = static_cast<std::size_t>(std::pow(duration, durationExponent) * durationMultiplier);
    this->sleepDuration = std::chrono::nanoseconds(durationInNs);

    this->updateSize = cmd::g_cmdParser.getIntArgument(cmd::constants::run_count_cmd,
                                                       cmd::constants::run_count_default);

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
    if (!this->running)
    {
        return;
    }

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

    this->updateCount++;
    if (this->updateCount >= this->updateSize)
    {
        globals::activeComponentCount--;
        this->running = false;
    }
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onEvent(const std::shared_ptr<nox::event::Event>& event)
{
    handleEvent(event);
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::onComponentEvent(const std::shared_ptr<nox::event::Event>& event)
{
    handleEvent(event);
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::serialize(Json::Value& /*componentObject*/)
{
}

template<std::size_t duration>
void 
components::TrivialComponent<duration>::handleEvent(const std::shared_ptr<nox::event::Event>& event)
{
    if (event->getType() == DummyEvent::ID)
    {
        auto dummyEvent = static_cast<DummyEvent*>(event.get());

        if (dummyEvent->receiverId == duration)
        {
            getLog().debug().format("Message sent from actor \"%s\" to actor \"%s\"", dummyEvent->senderName, NAME);
        }
    }
}
