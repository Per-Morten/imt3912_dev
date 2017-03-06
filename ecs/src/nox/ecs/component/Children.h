#ifndef NOX_ECS_CHILDREN_H_
#define NOX_ECS_CHILDREN_H_
#include <nox/ecs/Component.h>
#include <vector>

namespace nox
{
    namespace ecs
    {
        class Children
            : public Component
        {
        public:
            using nox::ecs::Component::Component;
            
            Children(Children&& other);
            Children& operator=(Children&& other);

            void addChild(const EntityId& childId);
            void removeChild(const EntityId& childId);

            bool hasChild(const EntityId& childId);
            
            EntityId operator[](std::size_t index) const;

            std::size_t size() const;
            bool empty() const;

        private:
            std::vector<EntityId> children{};
        };
    }
}



#endif
