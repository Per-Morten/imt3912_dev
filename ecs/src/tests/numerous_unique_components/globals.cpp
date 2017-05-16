#include <globals.h>
#ifdef NOX_USE_STRING_TYPE_ID

const std::string globals::dummy_event_sender_arg = "dummy_event_sender_arg";
const std::string globals::dummy_event_receiver_arg = "dummy_event_receiver_arg";
#endif

std::atomic<int> globals::activeComponentCount{0};

nox::ecs::EntityManager* globals::manager;


