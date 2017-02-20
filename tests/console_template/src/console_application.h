#pragma once
#include <nox/app/Application.h>
#include <nox/app/log/Logger.h>
#include <nox/logic/Logic.h>
#include <nox/util/Timer.h>

class ConsoleApplication 
    : public nox::app::Application
{
public:
    ConsoleApplication();

    virtual bool onInit() override;
    virtual void onUpdate(const nox::Duration& deltaTime) override;

private:
    bool initializeResourceCache();
    nox::logic::Logic* initializeLogic();
    nox::logic::physics::Simulation* initializePhysics(nox::logic::Logic* logic);
    nox::logic::world::Manager* initializeWorldManager(nox::logic::Logic* logic);

    bool loadWorldFile(nox::logic::IContext* logicContext, nox::logic::world::Manager* worldManager);

    nox::app::log::Logger log;
    nox::util::Timer<nox::Duration> outputTimer;

};
