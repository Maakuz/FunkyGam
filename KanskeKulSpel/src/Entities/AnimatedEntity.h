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
        AnimationData() {};
    };

    AnimatedEntity(AnimationData data, sf::Vector2f pos, int health = 100, int speed = 10, int attack = 10);
    AnimatedEntity(sf::Vector2f pos, int health = 100, int speed = 10, int attack = 10);
    virtual ~AnimatedEntity() {};

    void setAnimationData(AnimationData data);

protected:
    void updateAnimation(float dt);
    void setAnimationFrame(sf::Vector2i frame) { this->currentFrame = frame; };

private:
    sf::Vector2i currentFrame;

    float timer;

    sf::IntRect texRect;

    AnimationData data;
};