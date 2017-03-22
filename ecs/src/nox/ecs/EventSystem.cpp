#include <nox/ecs/EventSystem.h>

nox::ecs::EventSystem::EventSystem()
    : readHead(static_cast<EventNode*>(&head))
    , writeHead(static_cast<EventNode*>(&head))
{
}

nox::ecs::EventSystem::~EventSystem()
{
    auto itr = this->cast(this->head.next);

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
    new (this->writeHead) EventNode({this->argumentAllocator, eventType, senderId, receiverId});
    return this->writeHead->event;
}

nox::ecs::Event&
nox::ecs::EventSystem::readNextEvent()
{
    this->readHead = this->cast(this->readHead->next);
    return this->readHead->event;
}

void
nox::ecs::EventSystem::clear()
{
    this->readHead = this->cast(&this->head);
    this->writeHead = this->cast(&this->head);
    auto itr = this->cast(this->head.next);
    while (itr)
    {
        auto next = this->cast(itr->next);
        itr->~EventNode();
        this->nodeAllocator.deallocate(itr);
        itr = next;
    }
    this->nodeAllocator.clear();
    this->argumentAllocator.clear();
    this->head.next = nullptr;
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

