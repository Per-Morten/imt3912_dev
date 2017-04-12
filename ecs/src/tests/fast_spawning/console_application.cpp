#include <console_application.h>

#include <cassert>

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
#include <nox/logic/physics/box2d/Box2DSimulation.h>

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
ConsoleApplication::initializeLogic()
{
    auto logic = std::make_unique<nox::logic::Logic>();
    this->logicContext = logic.get();
    addProcess(std::move(logic));
    this->entityManager.setLogicContext(this->logicContext);
}

void
ConsoleApplication::initializePhysics()
{
    auto physics = std::make_unique<nox::logic::physics::Box2DSimulation>(this->logicContext);
    physics->setLogger(createLogger());
    auto physicsPtr = physics.get();
    this->logicContext->setPhysics(std::move(physics));
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
    
    initializeLogic();
    initializePhysics();

    const auto transformInfo = nox::ecs::createMetaInformation<nox::ecs::Transform>(nox::ecs::component_type::TRANSFORM);
    this->entityManager.registerComponent(transformInfo);
    const auto spriteInfo = nox::ecs::createMetaInformation<nox::ecs::Sprite>(nox::ecs::component_type::SPRITE);
    this->entityManager.registerComponent(spriteInfo);

    this->entityManager.configureComponents();

    const auto actorAmount = cmd::g_cmdParser.getIntArgument(cmd::constants::actor_amount_cmd,
                                                             cmd::constants::actor_amount_default);

    const auto deletionAmount = cmd::g_cmdParser.getIntArgument(cmd::constants::deletion_amount_cmd,
                                                                cmd::constants::deletion_amount_default);
    
    for (std::size_t i = 0; i < deletionAmount; ++i)
    {
        log.info().format("Creating world");
        for (std::size_t j = 0; j < actorAmount; ++j)
        {
            const auto id = this->entityManager.createEntity();
            this->entityManager.assignComponent(id, nox::ecs::component_type::TRANSFORM);
            this->entityManager.assignComponent(id, nox::ecs::component_type::SPRITE);
        }

        this->entityManager.createStep();
        this->entityManager.awakeStep();
        this->entityManager.activateStep();
        
        log.info().format("Deleting world");
        for (std::size_t j = 0; j < actorAmount; ++j)
        {
            this->entityManager.removeEntity(j + i * actorAmount);
        }
        
        this->entityManager.deactivateStep();
        this->entityManager.hibernateStep();
        this->entityManager.removeStep();
    }

    this->logicContext->pause(false);

    return true;
}

void 
ConsoleApplication::onUpdate(const nox::Duration& deltaTime)
{
    quitApplication();
}
