const glm::vec4& 
nox::ecs::Sprite::getColor() const
{
    return this->color;
}

const glm::vec2& 
nox::ecs::Sprite::getScale() const
{
    return this->scale;
}

const glm::vec2& 
nox::ecs::Sprite::getCenter() const
{
    return this->center;
}

void 
nox::ecs::Sprite::setColor(const glm::vec4& color)
{
    this->color = color;
    this->onColorChange(this->color);
}

void 
nox::ecs::Sprite::setScale(const glm::vec2& scale)
{
    if (this->scale != scale)
    {
        this->scale = scale;
        this->updateRenderTransform();
    }
}

void 
nox::ecs::Sprite::setOpacity(const float opacity)
{
    this->color.a = opacity;
    this->onColorChange(this->color);
}

void 
nox::ecs::Sprite::setRotation(const float rotation)
{
    if (this->rotation != rotation)
    {
        this->rotation = rotation;
        this->updateRenderTransform();
    }
}

void 
nox::ecs::Sprite::setPosition(const glm::vec2& position)
{
    if (this->position != position)
    {
        this->position = position;
        this->updateRenderTransform();
    }
}

void 
nox::ecs::Sprite::setFlippedHorizontal(const bool flipped)
{
    this->flipScale.x = (flipped == true) ? -1.0f : 1.0f;
    this->updateRenderTransform();
}

void 
nox::ecs::Sprite::setFlippedVertical(const bool flipped)
{
    this->flipScale.y = (flipped == true) ? -1.0f : 1.0f;
    this->updateRenderTransform();
}

void 
nox::ecs::Sprite::setOffset(const glm::vec2& offset)
{
    this->offset = offset;
    this->updateEntityTransform();
}
