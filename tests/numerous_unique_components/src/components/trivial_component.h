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

        virtual const IdType& getName() const override;
        virtual bool initialize(const Json::Value& componentJsonObject) override;
        virtual void onUpdate(const nox::Duration& deltaTime) override;
        virtual void serialize(Json::Value& componentObject) override;

    private:
        std::chrono::milliseconds sleepDuration{ durationInMs };

    };
}

#include "trivial_component.tpp"
