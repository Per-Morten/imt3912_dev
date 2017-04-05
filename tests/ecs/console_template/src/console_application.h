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
    virtual void onUpdate(const nox::Duration& deltaTime) override;

private:
    bool initializeResourceCache();
    nox::logic::Logic* initializeLogic();
    void initializeEntityManager();

    void loadWorld();

    nox::log::Logger log;
    nox::util::Timer<nox::Duration> outputTimer;

    nox::logic::Logic* logicContext{};
};

