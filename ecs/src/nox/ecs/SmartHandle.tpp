#include <nox/ecs/ComponentCollection.h>
#include <nox/ecs/UniquePtrComponentCollection.h>
#include <nox/ecs/VirtualComponentCollection.h>


template<class T, class Collection>
nox::ecs::SmartHandle<T, Collection>::SmartHandle(const EntityId& id,
                                                  T* object,
                                                  const std::size_t generation,
                                                  Collection* collection)
    : generation(generation)
    , id(id)
    , collection(collection)
    , ptr(object)
{

}

template<class T, class Collection>
T*
nox::ecs::SmartHandle<T, Collection>::get()
{
    if (this->generation != collection->getGeneration() || this->ptr == nullptr)
    {
        *this = collection->getComponent(this->id);
    }
    return this->ptr;
}

template<class T, class Collection>
T*
nox::ecs::SmartHandle<T, Collection>::operator*()
{
    return this->get();
}

template<class T, class Collection>
T*
nox::ecs::SmartHandle<T, Collection>::operator->()
{
    return this->get();
}

template<class T, class Collection>
template<class U, class Coll2>
nox::ecs::SmartHandle<T, Collection>::operator SmartHandle<U, Coll2>() const
{
    auto newPtr = (this->ptr == nullptr) ? nullptr : static_cast<U*>(this->ptr);
    SmartHandle<U, Coll2> other(this->id,
                                newPtr,
                                this->generation, 
                                this->collection);
    return other;
}
