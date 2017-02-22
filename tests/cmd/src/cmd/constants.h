#pragma once

namespace cmd
{
    namespace constants
    {
        constexpr auto thread_amount_cmd = "-thread_amount";
        constexpr auto thread_amount_default = 1;

        constexpr auto actor_amount_cmd = "-actor_amount";
        constexpr auto actor_amount_default = 10;

        constexpr auto actor_name_cmd = "-actor_name";
        constexpr auto actor_name_default = "EmptyActor";

        constexpr auto deletion_amount_cmd = "-deletion_amount";
        constexpr auto deletion_amount_default = 1;


        constexpr auto run_duration_ms_cmd = "-run_time_ms";
        constexpr auto run_duration_ms_default = 1000;

        constexpr auto world_path_cmd = "-world_path";
        constexpr auto world_path_default = "world/world.json";
    }
}

