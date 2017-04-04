#include <nox/ecs/component/Sprite.h>

#include <cassert>

#include <nox/app/graphics/2d/SpriteRenderNode.h>
#include <nox/app/graphics/2d/TransformationNode.h>
#include <nox/ecs/component/Types.h>
#include <nox/ecs/EntityManager.h>
#include <nox/event/IBroadcaster.h>
#include <nox/logic/graphics/event/SceneNodeEdited.h>
#include <nox/logic/IContext.h>
#include <nox/util/json_utils.h>

#include <glm/gtc/matrix_transform.hpp>

void 
nox::ecs::Sprite::initialize(const Json::Value& componentJsonObject)
{
    this->spriteRenderNode = std::make_shared<app::graphics::SpriteRenderNode>();

    this->scale = util::parseJsonVec(componentJsonObject["scale"], glm::vec2(1.0f));
    this->rotation = componentJsonObject.get("rotation", 0.0f).asFloat();
    this->flipScale = util::parseJsonVec(componentJsonObject["flipScale"], glm::vec2(1.0f));
    this->renderingEnabled = true;

    this->entityTransformNode = std::make_shared<app::graphics::TransformationNode>();
    this->renderTransformNode = std::make_shared<app::graphics::TransformationNode>();
    this->entityTransformNode->addChild(this->renderTransformNode);

    this->color = util::parseJsonVec(componentJsonObject["color"], glm::vec4(1.0f));

    this->lightMultiplier = componentJsonObject.get("lightMultiplier", 1.0f).asFloat();
    this->emissiveLight = componentJsonObject.get("emissiveLight", 0.0f).asFloat();

    this->onColorChange(this->color);
    this->onLightMultiplierChange(this->lightMultiplier);
    this->onEmissiveLightChange(this->emissiveLight);

    this->renderLevel = componentJsonObject.get("renderLevel", 0).asUInt();
    this->spriteName = componentJsonObject.get("spriteName", "").asString();
    this->center = util::parseJsonVec(componentJsonObject["center"], glm::vec2(0.5f));
    
    this->spriteRenderNode->setRenderLevel(this->renderLevel);
    this->spriteRenderNode->addSprite(this->spriteName);
    this->spriteRenderNode->setActiveSprite(this->spriteName);
    this->spriteRenderNode->setCenter(this->center);

    onCreate();
}

void 
nox::ecs::Sprite::serialize(Json::Value &componentObject)
{
    componentObject["scale"] = util::writeJsonVec(this->scale);
    componentObject["rotation"] = this->rotation;
    componentObject["flipScale"] = util::writeJsonVec(this->flipScale);

    componentObject["color"] = Json::objectValue;
    componentObject["color"] = util::writeJsonVec(this->color, util::JsonVecCoordType::COLOR);

    componentObject["spriteName"] = this->spriteName;
    componentObject["renderLevel"] = this->renderLevel;

    componentObject["center"] = util::writeJsonVec(this->center);
}

void 
nox::ecs::Sprite::deactivate()
{
    if (this->renderingEnabled == true)
    {
        this->broadcastSceneNodeRemoval();
    }
}

void 
nox::ecs::Sprite::activate()
{
    if (this->renderingEnabled == true)
    {
        this->broadcastSceneNodeCreation();
    }
}

void 
nox::ecs::Sprite::receiveEntityEvent(const ecs::Event& event)
{
    if (event.getType() == TypeIdentifier("TransformChange"))
    {
        this->updateEntityTransform();
    }
}

void 
nox::ecs::Sprite::setCenter(const glm::vec2& center)
{
    this->center = center;
    this->spriteRenderNode->setCenter(this->center);
}

void 
nox::ecs::Sprite::setActiveSprite(const std::string& spriteName)
{
    this->spriteRenderNode->setActiveSprite(spriteName);
}

void 
nox::ecs::Sprite::setRenderingState(const bool enable)
{
    if (enable == true && this->renderingEnabled == false)
    {
        this->broadcastSceneNodeCreation();
        this->renderingEnabled = true;
    }

    if (enable == false && this->renderingEnabled == true)
    {
        this->broadcastSceneNodeRemoval();
        this->renderingEnabled = false;
    }
}

void 
nox::ecs::Sprite::setRenderLevel(unsigned int renderLevel)
{
    if (this->renderLevel != renderLevel)
    {
        this->renderLevel = renderLevel;
        this->spriteRenderNode->setRenderLevel(renderLevel);
    }
}

void 
nox::ecs::Sprite::addSprite(const std::string& spriteName)
{
    this->spriteRenderNode->addSprite(spriteName);
}

void 
nox::ecs::Sprite::onCreate()
{
    auto tempHandle = this->entityManager->getComponent(this->id,
                                                        TypeIdentifier(component_types::TRANSFORM));
    this->transformHandle = static_cast<ComponentHandle<Transform>>(tempHandle);

    auto renderNode = this->createSceneNode();
    this->renderTransformNode->addChild(renderNode);

    this->updateRenderTransform();
}

void 
nox::ecs::Sprite::onColorChange(const glm::vec4& color)
{
    this->spriteRenderNode->setColor(color);
}

void 
nox::ecs::Sprite::onLightMultiplierChange(const float lightMultiplier)
{
    this->spriteRenderNode->setLightMultiplier(lightMultiplier);
}

void 
nox::ecs::Sprite::onEmissiveLightChange(const float emissiveLight)
{
    this->spriteRenderNode->setEmissiveLight(emissiveLight);
}

std::shared_ptr<nox::app::graphics::SceneGraphNode> 
nox::ecs::Sprite::createSceneNode() const
{
    return this->spriteRenderNode;
}

void 
nox::ecs::Sprite::updateRenderTransform()
{
    glm::mat4 transformation;
    transformation = glm::translate(transformation, glm::vec3(this->position, 0.0f));
    transformation = glm::rotate(transformation, this->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    transformation = glm::scale(transformation, glm::vec3(this->flipScale, 1.0f));
    transformation = glm::scale(transformation, glm::vec3(this->scale, 1.0f));
    this->renderTransformNode->setTransform(transformation);
}

void 
nox::ecs::Sprite::updateEntityTransform()
{
    const glm::vec2& position = this->transformHandle->getPosition();
    const glm::vec2& scale = this->transformHandle->getScale();
    const float rotation = this->transformHandle->getRotation();

    glm::mat4 transformMatrix;
    transformMatrix = glm::translate(transformMatrix, glm::vec3(this->offset, 0.0f));
    transformMatrix = glm::translate(transformMatrix, glm::vec3(position, 0.0f));
    transformMatrix = glm::rotate(transformMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    transformMatrix = glm::scale(transformMatrix, glm::vec3(scale, 1.0f));

    this->entityTransformNode->setTransform(transformMatrix);
}

void 
nox::ecs::Sprite::broadcastSceneNodeCreation()
{
    auto sceneNodeAddEvent = std::make_shared<nox::logic::graphics::SceneNodeEdited>(this->entityTransformNode, 
                                                                                     nox::logic::graphics::SceneNodeEdited::Action::CREATE);
    this->entityManager->getLogicContext()->getEventBroadcaster()->queueEvent(sceneNodeAddEvent);
}

void 
nox::ecs::Sprite::broadcastSceneNodeRemoval()
{
    auto sceneNodeRemoveEvent = std::make_shared<nox::logic::graphics::SceneNodeEdited>(this->entityTransformNode, 
                                                                                        nox::logic::graphics::SceneNodeEdited::Action::REMOVE);
    this->entityManager->getLogicContext()->getEventBroadcaster()->queueEvent(sceneNodeRemoveEvent);
}
