#pragma once

#include <nox/logic/actor/Component.h>
#include <nox/util/Timer.h>

namespace components
{
    class SpawningComponent
        : public nox::logic::actor::Component
    {
    public:
        static const IdType NAME;
    
        const IdType& getName() const override;
        bool initialize(const Json::Value& componentJsonObject) override;
        void onCreate() override;
        void onUpdate(const nox::Duration& deltaTime) override;
        void serialize(Json::Value& componentObject) override;
    
    private:   
        void createWorld();
        void deleteWorld();
        std::size_t deletionCounter{0};
    
    };
}
