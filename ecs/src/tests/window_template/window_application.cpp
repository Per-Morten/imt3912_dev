#include <window_application.h>

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

#include <nox/ecs/component/Transform.h>
#include <nox/ecs/createMetaInformation.h>
#include <nox/ecs/ComponentType.h>
#include <nox/ecs/component/Sprite.h>
#include <nox/ecs/SmartHandle.h>

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
    const auto projectAssetDirectory = "ecs/src/tests/" + getName() + "/assets";
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

void 
WindowApplication::initializeLogic()
{
    auto logic = std::make_unique<nox::logic::Logic>();
    this->logicContext = logic.get();
    addProcess(std::move(logic));
}

void
WindowApplication::initializePhysics()
{
    auto physics = std::make_unique<nox::logic::physics::Box2DSimulation>(this->logicContext);
    physics->setLogger(createLogger());
    this->logicContext->setPhysics(std::move(physics));
}

void 
WindowApplication::initializeWindow()
{
    // Create the window with this as the context and the applciation name as the window title.
    auto tempWindow = std::make_unique<WindowView>(this, getName());
    this->window = tempWindow.get();
    this->logicContext->addView(std::move(tempWindow));
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

    initializeLogic();
    this->entityManager.setLogicContext(this->logicContext);
    initializePhysics();
    initializeWindow();

    const auto transformInfo = nox::ecs::createMetaInformation<nox::ecs::Transform>(nox::ecs::component_type::TRANSFORM);
    this->entityManager.registerComponent(transformInfo);
    const auto spriteInfo = nox::ecs::createMetaInformation<nox::ecs::Sprite>(nox::ecs::component_type::SPRITE);
    this->entityManager.registerComponent(spriteInfo);

    this->entityManager.configureComponents();

    auto id = this->entityManager.createEntity();
    this->entityManager.assignComponent(id, nox::ecs::component_type::TRANSFORM);
    this->entityManager.assignComponent(id, nox::ecs::component_type::SPRITE);
    this->entityManager.awakeEntity(id);
    this->entityManager.activateEntity(id);


    this->logicContext->pause(false);

    return true;
}

void 
WindowApplication::onUpdate(const nox::Duration& deltaTime)
{
    SdlApplication::onUpdate(deltaTime);

    assert(window != nullptr);

    this->entityManager.step(deltaTime);

    // We need to manually render the window (the Logic doesn't know that it is a window and can render).
    this->window->render();
}

void 
WindowApplication::onSdlEvent(const SDL_Event& event)
{
    SdlApplication::onSdlEvent(event);

    assert(window != nullptr);

    // Pass the event to the window so that it can handle it.
    window->onSdlEvent(event);
}
