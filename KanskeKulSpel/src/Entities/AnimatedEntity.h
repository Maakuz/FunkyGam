#pragma once
#include "Entity.h"
#include "SFML/Graphics.hpp"

class AnimatedEntity : public Entity
{
public:
    struct AnimationData
    {
        sf::Texture * spriteSheet;
        sf::Vector2u frameCount;
        float animationSpeed;

        AnimationData(sf::Texture * spriteSheet, sf::Vector2u frameCount, float animationSpeed)
        {
            this->spriteSheet = spriteSheet;
            this->frameCount = frameCount;
            this->animationSpeed = animationSpeed;
        }
        AnimationData() 
        {
            this->animationSpeed = 0;
            this->spriteSheet = nullptr;
        };
    };

    AnimatedEntity(AnimationData data, sf::Vector2f pos);
    virtual ~AnimatedEntity() {};

    void flipHorizontally();

    void setAnimationData(AnimationData data);

protected:
    void updateAnimation(float dt);
    void setAnimationFrame(sf::Vector2u frame) { this->currentFrame = frame; };

private:
    sf::Vector2u currentFrame;

    float timer;

    sf::IntRect texRect;

    AnimationData data;
};