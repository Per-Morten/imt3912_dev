const glm::vec2& 
nox::ecs::Transform::getPosition() const
{
    return this->position;
}

float 
nox::ecs::Transform::getRotation() const
{
    return this->rotation;
}

const glm::vec2& 
nox::ecs::Transform::getScale() const
{
    return this->scale;
}
