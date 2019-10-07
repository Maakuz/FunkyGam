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
        sf::Vector2u idleFrame;

        AnimationData(sf::Texture * spriteSheet, sf::Vector2u frameCount, float animationSpeed, sf::Vector2u idleFrame = sf::Vector2u(0, 0))
        {
            this->spriteSheet = spriteSheet;
            this->frameCount = frameCount;
            this->animationSpeed = animationSpeed;
            this->idleFrame = idleFrame;
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
    void pauseAnimation();
    void resumeAnimation() { this->idle = false; };
    void updateAnimation(float dt);

    void setAnimationFrame(sf::Vector2u frame) { this->currentFrame = frame; };

    bool isIdle() const { return this->idle; }
private:
    sf::Vector2u currentFrame;
    bool idle;
    float timer;

    sf::IntRect texRect;

    AnimationData data;
};