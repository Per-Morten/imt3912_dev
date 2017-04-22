#pragma once
#include <atomic>

#include <nox/ecs/EntityManager.h>

namespace globals
{
    std::atomic<int> activeComponentCount{0};

    constexpr std::size_t dummy_event = 10;
    constexpr std::size_t dummy_event_sender_arg = 0;
    constexpr std::size_t dummy_event_receiver_arg = 1;

    constexpr std::size_t first_unreserved_id = 1000;

    nox::ecs::EntityManager* manager;
};
