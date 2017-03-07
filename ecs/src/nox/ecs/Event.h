#ifndef NOX_ECS_EVENT_H_
#define NOX_ECS_EVENT_H_
#include <vector>

#include <nox/ecs/EntityId.h>
#include <nox/ecs/EventArgument.h>
#include <nox/ecs/TypeIdentifier.h>

namespace nox
{
    namespace ecs
    {
        class Event
        {
        public:
            Event() = default; //todo: Delete this, keeping it default now for testing.

            Event(const EntityId& receiverId,
                  const TypeIdentifier& eventType);

            Event(const Event&) = delete;
            Event& operator=(const Event&) = delete;

            Event(Event&&) = default;
            Event& operator=(Event&&) = default;

            ~Event();

            void 
            addArgument(const TypeIdentifier& name,
                        EventArgument argument);

            bool
            hasArgument(const TypeIdentifier& name) const;

            const EventArgument&
            getArgument(const TypeIdentifier& name) const;

            const TypeIdentifier& 
            getType() const;

            const EntityId&
            getReceiver() const;

        private:
            struct ArgumentPair
            {
                TypeIdentifier name;
                EventArgument arg;
            };

            EntityId receiver;
            TypeIdentifier type{0};

            std::vector<ArgumentPair> arguments;
        };
    }
}

#endif
