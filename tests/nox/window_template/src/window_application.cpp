#include "window_application.h"

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

WindowApplication::WindowApplication()
    : SdlApplication("window_template", "PTPERF")
    , window(nullptr)
{
}

bool 
WindowApplication::initializeResourceCache()
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
WindowApplication::initializeLogic()
{
    auto logic = std::make_unique<nox::logic::Logic>();
    auto logicPtr = logic.get();

    addProcess(std::move(logic));

    return logicPtr;
}

nox::logic::physics::Simulation* 
WindowApplication::initializePhysics(nox::logic::Logic* logic)
{
    auto physics = std::make_unique<nox::logic::physics::Box2DSimulation>(logic);
    physics->setLogger(createLogger());

    auto physicsPtr = physics.get();

    logic->setPhysics(std::move(physics));

    return physicsPtr;
}

nox::logic::world::Manager* 
WindowApplication::initializeWorldManager(nox::logic::Logic* logic)
{
    auto world = std::make_unique<nox::logic::world::Manager>(logic);

    world->registerActorComponent<nox::logic::actor::Transform>();
    world->registerActorComponent<nox::logic::physics::ActorPhysics>();

    // Register the ActorSprite component so we can render sprites.
    world->registerActorComponent<nox::logic::graphics::ActorSprite>();

    const auto actorDirectory = std::string{"actor"};
    world->loadActorDefinitions(getResourceAccess(), actorDirectory);

    auto worldPtr = world.get();

    logic->setWorldManager(std::move(world));

    return worldPtr;
}

bool 
WindowApplication::loadWorldFile(nox::logic::IContext* logicContext, nox::logic::world::Manager* worldManager)
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

void 
WindowApplication::initializeWindow(nox::logic::Logic* logic)
{
    // Create the window with this as the context and the applciation name as the window title.
    auto tempWindow = std::make_unique<WindowView>(this, getName());

    window = tempWindow.get();

    /*
     * The window is actually not just a window, its also a logic::View. It is basically a logic::View that renders
     * the logic to a window and handles input from mouse and keyboard. Here we add it to the logic so it can manage it.
     *
     * The logic::View is a concept that represents a view into the logic. A view displays the logic to a user and
     * handles input from a user. A "user" can both be you viewing the application through a window, an AI viewing
     * the application through events and queries, a remote client viewing the application through a network connection,
     * or more.
     */
    logic->addView(std::move(tempWindow));
}

bool 
WindowApplication::onInit()
{
    if (SdlApplication::onInit() == false)
    {
        return false;
    }

    log = createLogger();
    log.setName("WindowApplication");

    if (initializeResourceCache() == false)
    {
        log.error().raw("Failed initializing resource cache.");
        return false;
    }

    auto logic = initializeLogic();
    logic->getEventBroadcaster();

    initializePhysics(logic);
    auto worldManager = initializeWorldManager(logic);

    // We initialize the window. This will make the window appear.
    initializeWindow(logic);

    if (loadWorldFile(logic, worldManager) == false)
    {
        return false;
    }

    logic->pause(false);

    return true;
}

void 
WindowApplication::onUpdate(const nox::Duration& deltaTime)
{
    SdlApplication::onUpdate(deltaTime);

    assert(window != nullptr);

    // We need to manually render the window (the Logic doesn't know that it is a window and can render).
    window->render();
}

void 
WindowApplication::onSdlEvent(const SDL_Event& event)
{
    SdlApplication::onSdlEvent(event);

    assert(window != nullptr);

    // Pass the event to the window so that it can handle it.
    window->onSdlEvent(event);
}
