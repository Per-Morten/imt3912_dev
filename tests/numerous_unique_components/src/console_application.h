#pragma once
#include <nox/app/Application.h>
#include <nox/log/Logger.h>
#include <nox/logic/Logic.h>
#include <nox/util/Timer.h>

class ConsoleApplication 
    : public nox::app::Application
{
public:
    ConsoleApplication();

    virtual bool onInit() override;

private:
    bool initializeResourceCache();
    nox::logic::Logic* initializeLogic();
    nox::logic::physics::Simulation* initializePhysics(nox::logic::Logic* logic);
    nox::logic::world::Manager* initializeWorldManager(nox::logic::Logic* logic);

    bool loadWorldFile(nox::logic::IContext* logicContext, nox::logic::world::Manager* worldManager);

    nox::log::Logger log;

};
