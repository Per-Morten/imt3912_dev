#pragma once

#include <chrono>

#include <nox/ecs/Component.h>
#include <nox/ecs/component/Transform.h>
#include <nox/ecs/SmartHandle.h>
#include <nox/ecs/Event.h>
#include <nox/ecs/EntityId.h>

namespace components
{
    template <std::size_t duration>
    class TrivialComponent
        : public nox::ecs::Component
    {
    public:
        TrivialComponent(const nox::ecs::EntityId& entityId,
                         nox::ecs::EntityManager* entityManager);

        void update(const nox::Duration& deltaTime);
        void receiveEntityEvent(const nox::ecs::Event& event);

    private:
        std::chrono::nanoseconds sleepDuration;
        std::size_t updateCount{ 0 };
        std::size_t updateSize{ 0 };
        bool running{ true };

    };
}

#include "trivial_component.tpp"
