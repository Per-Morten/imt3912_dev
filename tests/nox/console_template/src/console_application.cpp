#include "console_application.h"

#include <cassert>

#include <nox/app/resource/cache/LruCache.h>
#include <nox/app/resource/data/JsonExtraData.h>
#include <nox/app/resource/loader/JsonLoader.h>
#include <nox/app/resource/provider/BoostFilesystemProvider.h>
#include <nox/logic/actor/component/Transform.h>
#include <nox/logic/actor/event/TransformChange.h>
#include <nox/logic/graphics/actor/ActorSprite.h>
#include <nox/logic/physics/actor/ActorPhysics.h>
#include <nox/logic/physics/box2d/Box2DSimulation.h>
#include <nox/logic/world/Loader.h>
#include <nox/logic/world/Manager.h>

#include <json/value.h>
#include <glm/gtx/string_cast.hpp>

#include <cmd/parser.h>

ConsoleApplication::ConsoleApplication()
    : Application("console_template", "PTPERF")
{
}

bool 
ConsoleApplication::initializeResourceCache()
{
    const auto cacheSizeMb = 512u;
    auto resourceCache = std::make_unique<nox::app::resource::LruCache>(cacheSizeMb);

    resourceCache->setLogger(createLogger());

    // We need to get resources from the project specific assets.
    const auto projectAssetDirectory = "tests/nox/" + getName() + "/assets";
    if (resourceCache->addProvider(std::make_unique<nox::app::resource::BoostFilesystemProvider>(projectAssetDirectory)) == false)
    {
        log.error().format("Could not initialized resource cache to \"%s\".", projectAssetDirectory.c_str());
        return false;
    }

    // We need to get resources from the common assets.
    const auto commonAssetsDirectory = std::string{"assets"};
    if (resourceCache->addProvider(std::make_unique<nox::app::resource::BoostFilesystemProvider>(commonAssetsDirectory)) == false)
    {
        log.error().format("Could not initialized resource cache to \"%s\".", commonAssetsDirectory.c_str());
        return false;
    }

    // We need to get resources from the NOX assets.
    const auto noxAssetsDirectory = std::string{"nox-engine/assets"};
    if (resourceCache->addProvider(std::make_unique<nox::app::resource::BoostFilesystemProvider>(noxAssetsDirectory)) == false)
    {
        log.error().format("Could not initialized resource cache to \"%s\".", noxAssetsDirectory.c_str());
        return false;
    }

    resourceCache->addLoader(std::make_unique<nox::app::resource::JsonLoader>(createLogger()));

    setResourceCache(std::move(resourceCache));

    return true;
}

nox::logic::Logic* 
ConsoleApplication::initializeLogic()
{
    auto logic = std::make_unique<nox::logic::Logic>();
    auto logicPtr = logic.get();

    addProcess(std::move(logic));

    return logicPtr;
}

nox::logic::physics::Simulation* 
ConsoleApplication::initializePhysics(nox::logic::Logic* logic)
{
    auto physics = std::make_unique<nox::logic::physics::Box2DSimulation>(logic);
    physics->setLogger(createLogger());

    auto physicsPtr = physics.get();

    logic->setPhysics(std::move(physics));

    return physicsPtr;
}

nox::logic::world::Manager* 
ConsoleApplication::initializeWorldManager(nox::logic::Logic* logic)
{
    auto world = std::make_unique<nox::logic::world::Manager>(logic);

    // Register actors components here
    world->registerActorComponent<nox::logic::actor::Transform>();
    world->registerActorComponent<nox::logic::physics::ActorPhysics>();
    world->registerActorComponent<nox::logic::graphics::ActorSprite>();

    const auto actorDirectory = std::string{"actor"};
    world->loadActorDefinitions(getResourceAccess(), actorDirectory);

    auto worldPtr = world.get();

    logic->setWorldManager(std::move(world));

    return worldPtr;
}

bool 
ConsoleApplication::loadWorldFile(nox::logic::IContext* logicContext, nox::logic::world::Manager* worldManager)
{
    const auto worldFileDescriptor = nox::app::resource::Descriptor{"world/exampleWorld.json"};
    const auto worldFileHandle = getResourceAccess()->getHandle(worldFileDescriptor);

    if (worldFileHandle == nullptr)
    {
        log.error().format("Could not load world: %s", worldFileDescriptor.getPath().c_str());
        return false;
    }
    else
    {
        const auto jsonData = worldFileHandle->getExtraData<nox::app::resource::JsonExtraData>();

        if (jsonData == nullptr)
        {
            log.error().format("Could not get JSON data for world: %s", worldFileDescriptor.getPath().c_str());
            return false;
        }
        else
        {
            auto loader = nox::logic::world::Loader{logicContext};

            if (loader.loadWorld(jsonData->getRootValue(), worldManager) == false)
            {
                log.error().format("Failed loading world \"%s\".", worldFileDescriptor.getPath().c_str());
                return false;
            }
        }
    }

    log.verbose().format("Loaded world \"%s\"", worldFileDescriptor.getPath().c_str());

    return true;
}

bool 
ConsoleApplication::onInit()
{
    log = createLogger();
    log.setName("ConsoleApplication");

    if (initializeResourceCache() == false)
    {
        log.error().raw("Failed initializing resource cache.");
        return false;
    }

    auto logic = initializeLogic();
    logic->getEventBroadcaster();

    initializePhysics(logic);
    auto worldManager = initializeWorldManager(logic);

    outputTimer.setTimerLength(std::chrono::milliseconds(500));

    if (loadWorldFile(logic, worldManager) == false)
    {
        return false;
    }

    logic->pause(false);

    return true;
}

void 
ConsoleApplication::onUpdate(const nox::Duration& deltaTime)
{
    outputTimer.spendTime(deltaTime);

    if (outputTimer.timerReached() == true)
    {
        log.info().raw("Printing out text in update loop");
        
        outputTimer.subtractCycle();
    }
}
