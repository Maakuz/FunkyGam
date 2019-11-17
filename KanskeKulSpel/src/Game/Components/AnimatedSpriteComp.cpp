#include "AnimatedSpriteComp.h"

AnimatedSpriteComp::AnimatedSpriteComp(AnimationData data, sf::Vector2f pos):
    SpriteComp(data.spriteSheet, pos)
{
    setAnimationData(data);
    this->idle = false;
}

void AnimatedSpriteComp::flipHorizontally()
{
    texRect.width = abs(texRect.width);
    texRect.height = abs(texRect.height);
    texRect.left = currentFrame.x * texRect.width;
    texRect.top = currentFrame.y * texRect.height;

    texRect.left += texRect.width;
    setTextureRect(texRect);

    SpriteComp::flipHorizontally();
}

void AnimatedSpriteComp::setAnimationData(AnimationData data)
{
    this->data = data;

    this->texRect.width = this->data.spriteSheet->getSize().x / this->data.frameCount.x;
    this->texRect.height = this->data.spriteSheet->getSize().y / this->data.frameCount.y;

    this->currentFrame.x = 0;
    this->currentFrame.y = 0;

    this->timer = 0;

    this->texRect.left = 0;
    this->texRect.top = 0;

    setTextureRect(texRect);
}

void AnimatedSpriteComp::setAnimation(int animation)
{
    if (data.currentAnimation == animation)
        return;

    data.currentAnimation = animation;
    currentFrame = data.animations[animation].startFrame;
    idle = data.animations[animation].startIdle;
    timer = 0;

    texRect.width = abs(texRect.width);
    texRect.height = abs(texRect.height);
    texRect.left = currentFrame.x * texRect.width;
    texRect.top = currentFrame.y * texRect.height;

    if (isFlippedHorizontally())
    {
        texRect.left += texRect.width;
        texRect.width = -texRect.width;
    }
    setTextureRect(texRect);
}

void AnimatedSpriteComp::pauseAnimation()
{
    currentFrame = data.animations[data.currentAnimation].idleFrame;

    texRect.width = abs(texRect.width);
    texRect.height = abs(texRect.height);
    texRect.left = currentFrame.x * texRect.width;
    texRect.top = currentFrame.y * texRect.height;

    if (isFlippedHorizontally())
    {
        texRect.left += texRect.width;
        texRect.width = -texRect.width;
    }
    setTextureRect(texRect);

    this->idle = true;
}

void AnimatedSpriteComp::updateAnimation(float dt)
{
    if (idle)
        return;

    timer += dt;

    if (timer > data.animations[data.currentAnimation].animationSpeed)
    {
        timer = 0;
        currentFrame.x++;

        if (currentFrame.x > data.animations[data.currentAnimation].stopFrame.x)
        {
            currentFrame.x = 0;
            currentFrame.y++;

            if (currentFrame.y > data.animations[data.currentAnimation].stopFrame.y)
            {
                currentFrame = data.animations[data.currentAnimation].startFrame;
            }
        }

        texRect.width = abs(texRect.width);
        texRect.height = abs(texRect.height);
        texRect.left = currentFrame.x * texRect.width;
        texRect.top = currentFrame.y * texRect.height;

        if (isFlippedHorizontally())
        {
            texRect.left += texRect.width;
            texRect.width = -texRect.width;
        }
        setTextureRect(texRect);

    }
}
