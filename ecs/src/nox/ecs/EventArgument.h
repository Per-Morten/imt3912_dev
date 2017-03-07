#ifndef NOX_ECS_EVENTARGUMENT_H_
#define NOX_ECS_EVENTARGUMENT_H_
#include <nox/ecs/TypeIdentifier.h>

namespace nox
{
    namespace ecs
    {
        class EventArgument
        {
        public:
            using Byte = unsigned char;
            using Destructor = void(*)(Byte*);

            EventArgument() = delete;
            EventArgument(Byte* payload,
                          const TypeIdentifier& identifier,
                          Destructor destructor);
            
            EventArgument(const EventArgument&) = delete;
            EventArgument& operator=(const EventArgument&) = delete;

            EventArgument(EventArgument&&) = default;
            EventArgument& operator=(EventArgument&&) = default;
            
            // Think if this should be templated perhaps?
            // Doing void* rather than Byte* to avoid 
            void* 
            value() const;

            const TypeIdentifier& 
            getType() const;

            // Also think if this should just be a friend thing, rather than a public accessors.
            // Clear with Suttung.
            Destructor
            getDestructor() const;

        private:
            TypeIdentifier type;
            Byte* payload;
            Destructor destructor;
        };
    }
}

#endif
