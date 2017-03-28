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

void
nox::ecs::EventSystem::push(Event&& event)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    auto tmp = this->writeHead;
    this->writeHead = static_cast<EventNode*>(this->nodeAllocator.allocate(sizeof(EventNode)));
    tmp->next = this->writeHead;
    new (this->writeHead) EventNode(std::move(event));
}

bool
nox::ecs::EventSystem::pop(Event& event)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (!empty())
    {
        this->readHead = this->cast(this->readHead->next);
        event = std::move(this->readHead->event);
        return true;
    }

    return false;
}

void
nox::ecs::EventSystem::clear()
{
    std::lock_guard<std::mutex> lock(this->mutex);

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

