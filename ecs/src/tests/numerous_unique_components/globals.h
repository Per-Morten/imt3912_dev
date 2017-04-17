#pragma once
#include <nox/ecs/EntityManager.h>

namespace globals
{
    int activeComponentCount;

    constexpr std::size_t dummy_event = 10;
    constexpr std::size_t dummy_event_sender_arg = 0;
    constexpr std::size_t dummy_event_receiver_arg = 1;

    nox::ecs::EntityManager* manager;
};
