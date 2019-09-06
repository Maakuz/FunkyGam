#pragma once
#include "AnimatedEntity.h"

class Player : public AnimatedEntity 
{
public:
    Player(AnimationData data, sf::Vector2f pos = sf::Vector2f(0, 0));
    ~Player() {};

    void update(float dt);

private:
    void move(float dt);

    sf::Vector2i acceleration;
    sf::Vector2f momentum;
    float friction;

    float lightBounceFreq;
    float lightBounceHeight;

};