template<class T>
nox::ecs::SmartHandle<T>::SmartHandle(T* object,
                                      const std::size_t generation,
                                      const MetaInformation* metaInformation)
    : ptr(object)
    , generation(generation)
    , metaInformation(metaInformation)
{

}

template<class T>
T*
nox::ecs::SmartHandle<T>::get() const
{
    return this->ptr;
}

template<class T>
T*
nox::ecs::SmartHandle<T>::operator*() const
{
    return this->get();
}

template<class T>
T*
nox::ecs::SmartHandle<T>::operator->() const
{
    return this->get();
}

template<class T>
template<class U>
nox::ecs::SmartHandle<T>::operator SmartHandle<U>() const
{
    SmartHandle<U> other(static_cast<U*>(this->ptr),
                         this->generation, 
                         this->metaInformation);
    return other;
}
