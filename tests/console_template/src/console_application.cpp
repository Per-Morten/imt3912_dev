#include "console_application.h"

#include <nox/app/resource/cache/LruCache.h>
#include <nox/app/resource/provider/BoostFilesystemProvider.h>
#include <nox/app/resource/loader/JsonLoader.h>
#include <nox/app/resource/data/JsonExtraData.h>
#include <nox/logic/world/Manager.h>
#include <nox/logic/world/Loader.h>
#include <nox/logic/actor/component/Transform.h>
#include <nox/logic/actor/event/TransformChange.h>
#include <nox/logic/physics/box2d/Box2DSimulation.h>
#include <nox/logic/physics/actor/ActorPhysics.h>
#include <nox/logic/graphics/actor/ActorSprite.h>

#include <json/value.h>
#include <glm/gtx/string_cast.hpp>
#include <cassert>

pt::ConsoleApplication::ConsoleApplication():
    Application("console_template", "PTPERF")
{
}

bool 
pt::ConsoleApplication::initializeResourceCache()
{
    const auto cacheSizeMb = 512u;
    auto resourceCache = std::make_unique<nox::app::resource::LruCache>(cacheSizeMb);

    resourceCache->setLogger(this->createLogger());

    // We need to get resources from the project specific assets.
    const auto projectAssetDirectory = "tests/" + this->getName() + "/assets";
    if (resourceCache->addProvider(std::make_unique<nox::app::resource::BoostFilesystemProvider>(projectAssetDirectory)) == false)
    {
        this->log.error().format("Could not initialized resource cache to \"%s\".", projectAssetDirectory.c_str());
        return false;
    }

    // We need to get resources from the common assets.
    const auto commonAssetsDirectory = std::string{"assets"};
    if (resourceCache->addProvider(std::make_unique<nox::app::resource::BoostFilesystemProvider>(commonAssetsDirectory)) == false)
    {
        this->log.error().format("Could not initialized resource cache to \"%s\".", commonAssetsDirectory.c_str());
        return false;
    }

    // We need to get resources from the NOX assets.
    const auto noxAssetsDirectory = std::string{"nox-engine/assets"};
    if (resourceCache->addProvider(std::make_unique<nox::app::resource::BoostFilesystemProvider>(noxAssetsDirectory)) == false)
    {
        this->log.error().format("Could not initialized resource cache to \"%s\".", noxAssetsDirectory.c_str());
        return false;
    }

    resourceCache->addLoader(std::make_unique<nox::app::resource::JsonLoader>(this->createLogger()));

    this->setResourceCache(std::move(resourceCache));

    return true;
}

nox::logic::Logic* 
pt::ConsoleApplication::initializeLogic()
{
    auto logic = std::make_unique<nox::logic::Logic>();
    auto logicPtr = logic.get();

    this->addProcess(std::move(logic));

    return logicPtr;
}

nox::logic::physics::Simulation* 
pt::ConsoleApplication::initializePhysics(nox::logic::Logic* logic)
{
    auto physics = std::make_unique<nox::logic::physics::Box2DSimulation>(logic);
    physics->setLogger(this->createLogger());

    auto physicsPtr = physics.get();

    logic->setPhysics(std::move(physics));

    return physicsPtr;
}

nox::logic::world::Manager* 
pt::ConsoleApplication::initializeWorldManager(nox::logic::Logic* logic)
{
    auto world = std::make_unique<nox::logic::world::Manager>(logic);

    // Register actors components here
    world->registerActorComponent<nox::logic::actor::Transform>();

    const auto actorDirectory = std::string{"actor"};
    world->loadActorDefinitions(this->getResourceAccess(), actorDirectory);

    auto worldPtr = world.get();

    logic->setWorldManager(std::move(world));

    return worldPtr;
}

bool 
pt::ConsoleApplication::loadWorldFile(nox::logic::IContext* logicContext, nox::logic::world::Manager* worldManager)
{
    const auto worldFileDescriptor = nox::app::resource::Descriptor{"world/exampleWorld.json"};
    const auto worldFileHandle = this->getResourceAccess()->getHandle(worldFileDescriptor);

    if (worldFileHandle == nullptr)
    {
        this->log.error().format("Could not load world: %s", worldFileDescriptor.getPath().c_str());
        return false;
    }
    else
    {
        const auto jsonData = worldFileHandle->getExtraData<nox::app::resource::JsonExtraData>();

        if (jsonData == nullptr)
        {
            this->log.error().format("Could not get JSON data for world: %s", worldFileDescriptor.getPath().c_str());
            return false;
        }
        else
        {
            auto loader = nox::logic::world::Loader{logicContext};

            if (loader.loadWorld(jsonData->getRootValue(), worldManager) == false)
            {
                this->log.error().format("Failed loading world \"%s\".", worldFileDescriptor.getPath().c_str());
                return false;
            }
        }
    }

    this->log.verbose().format("Loaded world \"%s\"", worldFileDescriptor.getPath().c_str());

    return true;
}

bool 
pt::ConsoleApplication::onInit()
{
    this->log = this->createLogger();
    this->log.setName("ConsoleApplication");

    if (this->initializeResourceCache() == false)
    {
        this->log.error().raw("Failed initializing resource cache.");
        return false;
    }

    auto logic = this->initializeLogic();
    auto eventBroadcaster = logic->getEventBroadcaster();

    this->initializePhysics(logic);
    auto worldManager = this->initializeWorldManager(logic);

    if (this->loadWorldFile(logic, worldManager) == false)
    {
        return false;
    }

    logic->pause(false);

    return true;
}

void 
pt::ConsoleApplication::onUpdate(const nox::Duration& deltaTime)
{
    this->log.info().raw("Printing out text in update loop");
}
