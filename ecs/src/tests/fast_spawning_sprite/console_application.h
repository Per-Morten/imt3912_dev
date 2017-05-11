#pragma once
#include <nox/app/Application.h>
#include <nox/ecs/EntityManager.h>
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
    void initializeLogic();
    void initializePhysics();

    nox::log::Logger log;
    nox::logic::Logic* logicContext;
    nox::ecs::EntityManager entityManager;
};

