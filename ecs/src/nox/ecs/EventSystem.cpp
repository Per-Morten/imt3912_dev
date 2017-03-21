#include <nox/ecs/EventSystem.h>

nox::ecs::EventSystem::EventSystem()
    : head(static_cast<Base*>(nodeAllocator.allocate(sizeof(EventNode))))
    , readHead(static_cast<EventNode*>(head))
    , writeHead(static_cast<EventNode*>(head))
{
    new (this->head) Base();
}

nox::ecs::EventSystem::~EventSystem()
{
    auto itr = this->cast(this->head->next);
    this->head->~Base();
    this->nodeAllocator.deallocate(this->head);

    while (itr)
    {
        itr->~EventNode();
        this->nodeAllocator.deallocate(itr);
    }
}

nox::ecs::Event&
nox::ecs::EventSystem::createEvent(const TypeIdentifier& eventType,
                                   const EntityId& senderId,
                                   const EntityId& receiverId)
{
    auto tmp = this->writeHead;
    this->writeHead = static_cast<EventNode*>(this->nodeAllocator.allocate(sizeof(EventNode)));
    tmp->next = this->writeHead;
    new (this->writeHead) EventNode({eventType, senderId, receiverId});
    return this->writeHead->event;
}

nox::ecs::Event&
nox::ecs::EventSystem::readNextEvent()
{
    this->readHead = this->cast(this->readHead->next);
    return this->readHead->event;
}

void
nox::ecs::EventSystem::reset()
{
    this->readHead = this->cast(this->head);
    this->writeHead = this->cast(this->head);
    auto itr = this->cast(this->head->next);
    while (itr)
    {
        auto next = this->cast(itr->next);
        itr->~EventNode();
        this->nodeAllocator.deallocate(itr);
        itr = next;
    }
    this->nodeAllocator.reset();
    this->head->next = nullptr;
}

bool
nox::ecs::EventSystem::empty()
{
    return this->readHead == this->writeHead;
}

nox::ecs::EventSystem::EventNode* 
nox::ecs::EventSystem::cast(Base* ptr)
{
    return static_cast<EventNode*>(ptr);
}

