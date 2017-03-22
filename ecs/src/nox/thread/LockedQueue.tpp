template<class T>
void 
nox::thread::LockedQueue<T>::push(const_reference value)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->queue.push(value); 
}

template<class T>
void 
nox::thread::LockedQueue<T>::push(value_type&& value)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->queue.push(std::move(value)); 
}


template<class T>
bool 
nox::thread::LockedQueue<T>::pop(reference value)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (!this->queue.empty())
    {
        value = std::move(this->queue.front());
        this->queue.pop();
        return true;
    }

    return false;
}

template<class T>
void 
nox::thread::LockedQueue<T>::clear()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    while (!this->queue.empty())
    {
        this->queue.pop();
    }
}

template<class T>
typename
nox::thread::LockedQueue<T>::size_type 
nox::thread::LockedQueue<T>::size() const
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->queue.size();
}

template<class T>
bool 
nox::thread::LockedQueue<T>::empty() const
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->queue.empty();
}


