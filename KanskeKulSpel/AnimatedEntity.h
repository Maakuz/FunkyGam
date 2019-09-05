#pragma once
#include "Entity.h"
#include "SFML/Graphics.hpp"

class AnimatedEntity : public Entity
{
public:
    struct AnimationData
    {
        sf::Texture * spriteSheet;
        sf::Vector2i sheetSize;
        float animationSpeed;
    };

    AnimatedEntity(AnimationData data, sf::Vector2f pos, int health = 100, int speed = 10, int attack = 10);
    ~AnimatedEntity() {};

    void update(float dt);
private:
    sf::Vector2i currentFrame;
    sf::Vector2i frameSize;
    float timer;

    AnimationData data;
};