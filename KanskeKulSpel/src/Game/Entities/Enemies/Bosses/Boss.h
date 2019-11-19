#pragma once
#include "Game/Entities/Enemies/Enemy.h"

class Boss : public Enemy
{
public:
    Boss(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset)
        :Enemy(data, pos, size, offset) {};

    virtual bool isInterrupting() = 0;
};