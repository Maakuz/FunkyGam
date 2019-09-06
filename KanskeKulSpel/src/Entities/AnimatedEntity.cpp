#include "AnimatedEntity.h"

AnimatedEntity::AnimatedEntity(AnimationData data, sf::Vector2f pos)
    :Entity(pos, data.spriteSheet)
{
    setAnimationData(data);
    this->addCollision();
}

void AnimatedEntity::setAnimationData(AnimationData data)
{
    this->data = data;
    setTexture(*this->data.spriteSheet);

    this->texRect.width = this->data.spriteSheet->getSize().x / this->data.frameCount.x;
    this->texRect.height = this->data.spriteSheet->getSize().y / this->data.frameCount.y;

    this->currentFrame.x = 0;
    this->currentFrame.y = 0;

    this->timer = 0;

    this->texRect.left = 0;
    this->texRect.top = 0;

    setTextureRect(texRect);
}

void AnimatedEntity::updateAnimation(float dt)
{
    timer += dt;

    if (timer > data.animationSpeed)
    {
        timer = 0;
        currentFrame.x++;

        if (currentFrame.x >= data.frameCount.x)
        {
            currentFrame.x = 0;
            currentFrame.y++;

            if (currentFrame.y >= data.frameCount.y)
            {
                currentFrame.y = 0;
            }
        }
        
        texRect.left = currentFrame.x * texRect.width;
        texRect.top = currentFrame.y * texRect.height;
        
        setTextureRect(texRect);
    }
}
