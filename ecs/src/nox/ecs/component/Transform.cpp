#include <nox/ecs/component/Transform.h>

#include <nox/ecs/createEventArgument.h>
#include <nox/ecs/EntityManager.h>
#include <nox/ecs/Event.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/util/json_utils.h>
#include <nox/ecs/EventType.h>

#include <glm/gtc/matrix_transform.hpp>

void 
nox::ecs::Transform::initialize(const Json::Value& componentJsonObject)
{
    this->position = util::parseJsonVec(componentJsonObject["position"], glm::vec2(0.0f, 0.0f));
    this->scale = util::parseJsonVec(componentJsonObject["scale"], glm::vec2(1.0f, 1.0f));
    this->rotation = componentJsonObject.get("rotation", 0.0f).asFloat();
}

void 
nox::ecs::Transform::serialize(Json::Value& componentObject)
{
    componentObject["position"] = util::writeJsonVec(this->position);
    componentObject["scale"] = util::writeJsonVec(this->scale);
    componentObject["rotation"] = this->rotation;
}

void 
nox::ecs::Transform::onCreate()
{
    this->broadcastTransformChange();
}

glm::mat4 
nox::ecs::Transform::getTransformMatrix() const
{
    glm::mat4 transform;
    transform = glm::translate(transform, glm::vec3(this->position, 0.0f));
    transform = glm::rotate(transform, this->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(this->scale, 1.0f));

    return transform;
}

void 
nox::ecs::Transform::setPosition(const glm::vec2& position, 
                                 bool broadcast)
{
    this->position = position;

    if (broadcast == true)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::setScale(const glm::vec2& scale, 
                              bool broadcast)
{
    this->scale = scale;

    if (broadcast == true)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::setRotation(const float rotation, 
                                 bool broadcast)
{
    this->rotation = rotation;

    if (broadcast == true)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::setTransform(const glm::vec2& position, 
                                  const float rotation, 
                                  const glm::vec2& scale, 
                                  bool broadcast)
{
    this->position = position;
    this->scale = scale;
    this->rotation = rotation;

    if (broadcast == true)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::broadcastTransformChange()
{
    auto event = std::move(this->entityManager->createEntityEvent({event_type::TRANSFORM_CHANGE},
                                                                  this->id,
                                                                  this->id));

    createEventArgument(event, this->position, {event_arg_type::TRANSFORM_CHANGE_POSITION});
    createEventArgument(event, this->rotation, {event_arg_type::TRANSFORM_CHANGE_ROTATION});
    createEventArgument(event, this->scale, {event_arg_type::TRANSFORM_CHANGE_SCALE});

    this->entityManager->sendEntityEvent(std::move(event));
}
