#include <string>

#define PT_DONT_OPTIMIZE asm("");

template<std::size_t durationInMs>
const nox::logic::actor::Component::IdType components::TrivialComponent<durationInMs>::NAME = std::string("TrivialComponent" + std::to_string(durationInMs));

template<std::size_t durationInMs>
const nox::logic::actor::Component::IdType& 
components::TrivialComponent<durationInMs>::getName() const
{
    return NAME;
}

template<std::size_t durationInMs>
bool 
components::TrivialComponent<durationInMs>::initialize(const Json::Value& /*componentJsonObject*/)
{
    return true;
}

template<std::size_t durationInMs>
void 
components::TrivialComponent<durationInMs>::onUpdate(const nox::Duration& deltaTime)
{
    const auto clockStart = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds currentDuration(0);

    while (currentDuration < sleepDuration)
    {
        PT_DONT_OPTIMIZE

        const auto now = std::chrono::high_resolution_clock::now();
        currentDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - clockStart);
    }
}

template<std::size_t durationInMs>
void 
components::TrivialComponent<durationInMs>::serialize(Json::Value& /*componentObject*/)
{
}
