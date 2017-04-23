#pragma once

#include <nox/app/SdlApplication.h>
#include <nox/ecs/EntityManager.h>
#include <nox/log/Logger.h>
#include <nox/logic/Logic.h>

#include <window_view.h>

class WindowApplication
    : public nox::app::SdlApplication
{
public:
    WindowApplication();

    bool onInit() override;
    virtual void onUpdate(const nox::Duration& deltaTime) override;
    virtual void onSdlEvent(const SDL_Event& event) override;

private:
    bool initializeResourceCache();
    void initializeLogic();
    void initializePhysics();
    void initializeWindow();

    nox::log::Logger log;
    WindowView* window;
    nox::logic::Logic* logicContext;
    nox::ecs::EntityManager entityManager;
};
