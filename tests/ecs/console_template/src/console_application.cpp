#include "console_application.h"

#include <cmd/parser.h>
#include <nox/app/resource/cache/LruCache.h>
#include <nox/app/resource/data/JsonExtraData.h>
#include <nox/app/resource/loader/JsonLoader.h>
#include <nox/app/resource/provider/BoostFilesystemProvider.h>
#include <nox/ecs/component/Sprite.h>
#include <nox/ecs/component/Transform.h>
#include <nox/ecs/ComponentType.h>
#include <nox/ecs/createMetaInformation.h>
#include <nox/ecs/SmartHandle.h>

#include <json/value.h>
#include <glm/gtx/string_cast.hpp>


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
    const auto projectAssetDirectory = "tests/ecs/" + getName() + "/assets";
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

void
ConsoleApplication::initializeEntityManager()
{
//    const auto transformInfo = nox::ecs::createMetaInformation<nox::ecs::Transform>(nox::ecs::component_type::TRANSFORM);
//    this->entityManager.registerComponent(transformInfo);
//
//    const auto spriteInfo = nox::ecs::createMetaInformation<nox::ecs::Sprite>(nox::ecs::component_type::SPRITE);
//    this->entityManager.registerComponent(spriteInfo);
}

bool 
ConsoleApplication::loadWorld()
{
//    auto id = this->entityManager.createEntity();
//    this->entityManager.assignComponent(id, nox::ecs::component_type::TRANSFORM);
//    this->entityManager.assignComponent(id, nox::ecs::component_type::SPRITE);
//    this->entityManager.awakeEntity(id);
//    this->entityManager.activateEntity(id);
}

bool 
ConsoleApplication::onInit()
{
    outputTimer.setTimerLength(std::chrono::milliseconds(500));

    log = createLogger();
    log.setName("ConsoleApplication");

    if (initializeResourceCache() == false)
    {
        log.error().raw("Failed initializing resource cache.");
        return false;
    }

    auto logic = initializeLogic();
    auto eventBroadcaster = logic->getEventBroadcaster();
    auto worldManager = initializeWorldManager();
    loadWorld();

    logic->pause(false);

    return true;
}

void 
ConsoleApplication::onUpdate(const nox::Duration& deltaTime)
{
    //this->entityManager.step();

    outputTimer.spendTime(deltaTime);

    if (outputTimer.timerReached() == true)
    {
        log.info().raw("Printing out text in update loop");
        
        outputTimer.subtractCycle();
    }
}
