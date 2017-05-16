template<class T>
typename std::enable_if<std::is_pointer<T>::value &&
                        std::is_const<std::remove_pointer_t<T>>::value,
                        T>::type
nox::ecs::Event::Argument::value() const
{
    return reinterpret_cast<T>(this->payload);
}

template<class T>
typename std::enable_if<!std::is_reference<T>::value &&
                        !std::is_pointer<T>::value,
                        T>::type
nox::ecs::Event::Argument::value() const
{
    return *reinterpret_cast<const T*>(this->payload);
}
