#ifndef NOX_ECS_PARENT_H_
#define NOX_ECS_PARENT_H_
#include <nox/ecs/Component.h>

namespace nox
{
    namespace ecs
    {
        class Parent
            : public Component
        {
        public:
            using nox::ecs::Component::Component;

            Parent(Parent&& other);
            Parent& operator=(Parent&& other);

            EntityId parentId{};
        };
    }
}

#endif
