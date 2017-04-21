#include <string>

#include <cmd/parser.h>
#include <globals.h>
#include <send_dummy_event.h>

#include <nox/util/pms_debug.h>


template<std::size_t duration>
components::TrivialComponent<duration>::TrivialComponent(const nox::ecs::EntityId& entityId,
                                                         nox::ecs::EntityManager* entityMnaager)
    : Component(entityId, entityManager)
{
    this->sleepDuration = std::chrono::nanoseconds(duration);
    this->updateSize = cmd::g_cmdParser.getIntArgument(cmd::constants::run_count_cmd,
                                                       cmd::constants::run_count_default);
    this->running = true;

    if (!this->entityManager)
    {
        PMS_DEBUG("Constructed with nullptr\n");
    }
    else
    {
        PMS_DEBUG("Constructed without nullptr\n");
    }
}

template<std::size_t duration>
components::TrivialComponent<duration>::TrivialComponent(TrivialComponent&& other)
        : nox::ecs::Component(other.id, other.entityManager)
        , updateSize(other.updateSize)
        , sleepDuration(other.sleepDuration)
        , updateCount(other.updateCount)
{
    if (!this->entityManager)
    {
        PMS_DEBUG("EntityManager is nullptr\n");
    }
}

template<std::size_t duration>
components::TrivialComponent<duration>&
components::TrivialComponent<duration>::operator=(TrivialComponent&& other)
{
    if (this != &other)
    {
        nox::ecs::Component::operator=(std::move(other));
        std::swap(this->updateSize, other.updateSize);
        std::swap(this->sleepDuration, other.sleepDuration);
        std::swap(this->updateCount, other.updateCount);
    }
    if (!this->entityManager)
    {
        PMS_DEBUG("EntityManager is nullptr\n");
    }

    return *this;
}

template<std::size_t duration>
void
components::TrivialComponent<duration>::update(const nox::Duration& deltaTime)
{
    if (this->running == false)
    {
        return;
    }

    const auto clockStart = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds currentDuration(0);

    sendDummyEvent<duration>(this->id,
                             this->entityManager);

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
components::TrivialComponent<duration>::receiveEntityEvent(const nox::ecs::Event& event)
{
    if (event.getType().getValue() == globals::dummy_event)
    {
        const auto receiverId = event.getArgument(globals::dummy_event_receiver_arg).value<std::size_t>();
        const auto senderId = event.getArgument(globals::dummy_event_sender_arg).value<std::size_t>();

        if (receiverId == duration)
        {
            printf("Message sent from actor \"%i\" to actor \"%i\"\n", (int)senderId, (int)this->id);
        }
    }
}
