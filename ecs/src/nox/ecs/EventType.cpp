#ifdef NOX_USE_STRING_TYPE_ID
#include <nox/ecs/EventType.h>

const std::string nox::ecs::event_type::TRANSFORM_CHANGE = "TRANSFORM_CHANGE";

const std::string nox::ecs::event_arg_type::TRANSFORM_CHANGE_POSITION = "TRANSFORM_CHANGE_POSITION";
const std::string nox::ecs::event_arg_type::TRANSFORM_CHANGE_ROTATION = "TRANSFORM_CHANGE_ROTATION";
const std::string nox::ecs::event_arg_type::TRANSFORM_CHANGE_SCALE = "TRANSFORM_CHANGE_SCALE";
#endif
