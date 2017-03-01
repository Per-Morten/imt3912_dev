#pragma once

#include <chrono>

#include <nox/logic/actor/Component.h>
#include <nox/event/IListener.h>
#include <nox/event/ListenerManager.h>

namespace components
{
    template <std::size_t duration>
    class TrivialComponent
        : public nox::logic::actor::Component
        , public nox::event::IListener
    {
    public:
        TrivialComponent();

        static const IdType NAME;

        virtual const IdType& getName() const override;
        virtual bool initialize(const Json::Value& componentJsonObject) override;
        virtual void onCreate() override;
        virtual void onActivate() override;
        virtual void onDeactivate() override;
        virtual void onUpdate(const nox::Duration& deltaTime) override;
        virtual void onEvent(const std::shared_ptr<nox::event::Event>& event) override;
        virtual void serialize(Json::Value& componentObject) override;

    private:
        std::chrono::nanoseconds sleepDuration{ duration * 100 };
        nox::event::ListenerManager listener;

    };
}

#include "trivial_component.tpp"
