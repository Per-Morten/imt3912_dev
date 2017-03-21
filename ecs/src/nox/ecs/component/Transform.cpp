#include "Transform.h"

#include <nox/util/json_utils.h>
#include <nox/ecs/Event.h>
#include <nox/ecs/TypeIdentifier.h>
#include <nox/ecs/createEventArgument.h>

#include <glm/gtc/matrix_transform.hpp>


const std::string nox::ecs::Transform::NAME = "Transform";


bool 
nox::ecs::Transform::initialize(const Json::Value& componentJsonObject)
{
    this->position = util::parseJsonVec(componentJsonObject["position"], glm::vec2(0.0f, 0.0f));
    this->scale = util::parseJsonVec(componentJsonObject["scale"], glm::vec2(1.0f, 1.0f));
    this->rotation = componentJsonObject.get("rotation", 0.0f).asFloat();

    return true;
}

void 
nox::ecs::Transform::onCreate()
{
    this->broadcastTransformChange();
}

glm::mat4 
nox::ecs::Transform::getTransformMatrix()
{
    glm::mat4 transform;
    transform = glm::translate(transform, glm::vec3(this->position, 0.0f));
    transform = glm::rotate(transform, this->rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, glm::vec3(this->scale, 1.0f));

    return transform;
}

const std::string& 
nox::ecs::Transform::getName() const
{
    return NAME;
}

void 
nox::ecs::Transform::setPosition(const glm::vec2& position, 
                                 bool broadcast)
{
    this->position = position;

    if (broadcast)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::setScale(const glm::vec2& scale, 
                              bool broadcast)
{
    this->scale = scale;

    if (broadcast)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::setRotation(const float rotation, 
                                 bool broadcast)
{
    this->rotation = rotation;

    if (broadcast)
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

    if (broadcast)
    {
        this->broadcastTransformChange();
    }
}

void 
nox::ecs::Transform::broadcastTransformChange()
{
    TypeIdentifier eventIdentifier("transformChange");
    Event event(eventIdentifier, this->id);

    TypeIdentifier positionType("transformChangePosition");
    TypeIdentifier rotationType("transformChangeRotation");
    TypeIdentifier scaleType("transformChangeScale");

    auto positionArgument = createEventArgument<glm::vec2>(this->position, positionType);
    auto rotationArgument = createEventArgument<float>(this->rotation, rotationType);
    auto scaleArgument = createEventArgument<glm::vec2>(this->scale, scaleType);

    event.addArgument(positionType,
                      std::move(positionArgument));
    event.addArgument(rotationType,
                      std::move(rotationArgument));
    event.addArgument(scaleType,
                      std::move(scaleArgument));

    this->entityManager->sendEntityEvent(std::move(event));
}

//void 
//nox::ecs::Transform::serialize(Json::Value& componentObject)
//{
//    componentObject["position"] = util::writeJsonVec(this->position);
//    componentObject["scale"] = util::writeJsonVec(this->scale);
//    componentObject["rotation"] = this->rotation;
//}
