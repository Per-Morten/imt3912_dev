#pragma once
#include <atomic>

#include <nox/ecs/EntityManager.h>

namespace globals
{
    extern std::atomic<int> activeComponentCount;

    constexpr std::size_t dummy_event = 10;

    #ifdef NOX_USE_STRING_TYPE_ID
    #pragma message "Using in numerous test: NOX_USE_STRING_TYPE_ID"
        const extern std::string dummy_event_sender_arg;
        const extern std::string dummy_event_receiver_arg;

    #else
    #pragma message "NOT Using in numerous test: NOX_USE_STRING_TYPE_ID"
        constexpr std::size_t dummy_event_sender_arg = 0;
        constexpr std::size_t dummy_event_receiver_arg = 1;
    #endif

    constexpr std::size_t first_unreserved_id = 1000;

    extern nox::ecs::EntityManager* manager;
};
