#include <string>

#include <cmd/parser.h>
#include <globals.h>
#include <send_dummy_event.h>

template<std::size_t duration>
components::TrivialComponent<duration>::TrivialComponent(const nox::ecs::EntityId& entityId,
                                                         nox::ecs::EntityManager* entityManager)
    : nox::ecs::Component(entityId, entityManager)
{
    this->sleepDuration = std::chrono::nanoseconds(duration - globals::first_unreserved_id);
    this->updateSize = cmd::g_cmdParser.getIntArgument(cmd::constants::run_count_cmd,
                                                       cmd::constants::run_count_default);
    this->running = true;

    globals::activeComponentCount++;
}

template<std::size_t duration>
void
components::TrivialComponent<duration>::update(const nox::Duration& /*deltaTime*/)
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
            printf("Message sent from actor \"%zu\" to actor \"%zu\"\n", senderId, this->id);
        }
    }
}
