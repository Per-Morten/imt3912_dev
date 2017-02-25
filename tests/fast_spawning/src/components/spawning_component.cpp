#include "spawning_component.h"

#include <nox/logic/Logic.h>
#include <nox/logic/IContext.h>
#include <nox/logic/event/IBroadcaster.h>
#include <nox/logic/actor/Actor.h>
#include <nox/logic/world/Manager.h>
#include <nox/logic/world/Loader.h>
#include <nox/logic/physics/Simulation.h>
#include <nox/logic/physics/actor/ActorPhysics.h>
#include <cmd/parser.h>
#include <nox/app/resource/cache/LruCache.h>
#include <nox/app/resource/data/JsonExtraData.h>
#include <nox/app/resource/loader/JsonLoader.h>
#include <nox/app/resource/provider/BoostFilesystemProvider.h>
#include <nox/logic/world/Loader.h>

#include "../console_application.h"

const components::SpawningComponent::IdType components::SpawningComponent::NAME = "SpawningComponent";

const components::SpawningComponent::IdType& components::SpawningComponent::getName() const
{
    return components::SpawningComponent::NAME;
}

bool components::SpawningComponent::initialize(const Json::Value& /*componentJsonObject*/)
{
    return true;
}

void components::SpawningComponent::onCreate()
{
}

void components::SpawningComponent::onUpdate(const nox::Duration& deltaTime)
{
    createWorld();
    deleteWorld();
}

void components::SpawningComponent::serialize(Json::Value& /*componentObject*/)
{
}


void components::SpawningComponent::createWorld()
{
    getLog().info().format("Creating world");
    auto actorAmount = cmd::g_cmdParser.getIntArgument(cmd::constants::actor_amount_cmd,
                                                       cmd::constants::actor_amount_default);

    auto actorName = cmd::g_cmdParser.getStringArgument(cmd::constants::actor_name_cmd,
                                                        cmd::constants::actor_name_default);

    auto worldManager = getLogicContext()->getWorldManager();

    for (int i = 0; i < actorAmount; ++i)
    {
        auto newActor = worldManager->createActorFromDefinitionName(actorName);
        worldManager->manageActor(std::move(newActor));
    }
}

void components::SpawningComponent::deleteWorld()
{
    getLog().info().format("Deleting world");
    auto actorAmount = cmd::g_cmdParser.getIntArgument(cmd::constants::actor_amount_cmd,
                                                       cmd::constants::actor_amount_default);
    auto deletionAmount = cmd::g_cmdParser.getIntArgument(cmd::constants::deletion_amount_cmd,
                                                          cmd::constants::deletion_amount_default);
    auto worldManager = getLogicContext()->getWorldManager();
    
    for (std::size_t i = 0; i < actorAmount; ++i)
    {
        //Needs to calculate the new ids by hand to figure out what actor to delete
        //+2 because 0 is for undefined ids, 1 is for the spawner itself, so 2 is the starting id
        //Also use deletionCounter * actorAmount as after each create delete cycle the id counter is not reset
        worldManager->removeActor(nox::logic::actor::Identifier(i + deletionCounter * actorAmount + 2));
    }

    deletionCounter++;

    if (deletionCounter >= deletionAmount)
    {
        auto logic = static_cast<nox::logic::Logic*>(getLogicContext());
        auto consoleApplication = static_cast<ConsoleApplication*>(logic->getApplicationContext());
        consoleApplication->quitApplication();
    }
}
