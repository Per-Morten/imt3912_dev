#pragma once

#include <chrono>

#include <nox/logic/actor/Component.h>

namespace components
{
    template <std::size_t durationInMs>
    class TrivialComponent
        : public nox::logic::actor::Component
    {
    public:
        static const IdType NAME;

        const IdType& getName() const override;
        bool initialize(const Json::Value& componentJsonObject) override;
        void onUpdate(const nox::Duration& deltaTime) override;
        void serialize(Json::Value& componentObject) override;

    private:
        std::chrono::milliseconds sleepDuration{ std::chrono::milliseconds(durationInMs) };

    };
}

#include "trivial_component.tpp"
