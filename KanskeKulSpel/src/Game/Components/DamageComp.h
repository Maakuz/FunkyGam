#pragma once
#include "SFML/System/Vector2.hpp"

class DamageComp
{
public:
    enum class DamageOrigin 
    {
        player,
        enemies,
        neutral
    };

    DamageComp(DamageOrigin origin) 
    {
        damage = 0;
        this->origin = origin;
    };

    virtual ~DamageComp() {};

    DamageOrigin origin;
    int damage;
    sf::Vector2f knockback;
};