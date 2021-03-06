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


#include <components/trivial_component.h>
#include <globals.h>
#include <recursive_register_component.h>

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
    globals::manager = &this->entityManager;
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

    //Register all trivial component templates
    registerTrivialComponent<TRIVIAL_COMPONENT_COUNT>(this->entityManager);

    this->entityManager.configureComponents();

    const auto actorAmount = static_cast<std::size_t>(cmd::g_cmdParser.getIntArgument(cmd::constants::actor_amount_cmd,
                                                                                      cmd::constants::actor_amount_default));

    for (std::size_t i = 0; i < actorAmount; ++i)
    {
        auto id = this->entityManager.createEntity();

        for (std::size_t j = globals::first_unreserved_id; j < TRIVIAL_COMPONENT_COUNT + globals::first_unreserved_id; ++j)
        {
            this->entityManager.assignComponent(id, j);
        }
        
        this->entityManager.awakeEntity(id);
        this->entityManager.activateEntity(id);
    }

    this->logicContext->pause(false);

    return true;
}

void 
ConsoleApplication::onUpdate(const nox::Duration& deltaTime)
{
    this->entityManager.step(deltaTime);
    if (globals::activeComponentCount <= 0)
    {
        quitApplication();
    }
}
