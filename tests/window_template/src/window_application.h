#pragma once

#include "window_view.h"

#include <nox/app/SdlApplication.h>
#include <nox/app/log/Logger.h>
#include <nox/logic/Logic.h>


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
    nox::logic::Logic* initializeLogic();
    nox::logic::physics::Simulation* initializePhysics(nox::logic::Logic* logic);
    nox::logic::world::Manager* initializeWorldManager(nox::logic::Logic* logic);

    void initializeWindow(nox::logic::Logic* logic);

    bool loadWorldFile(nox::logic::IContext* logicContext, nox::logic::world::Manager* worldManager);

    nox::app::log::Logger log;

    // Store the window so we can update it.
    WindowView* window;
};
