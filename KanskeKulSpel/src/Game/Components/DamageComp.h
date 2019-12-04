#pragma once
#include "SFML/System/Vector2.hpp"
#include "Game/Components/Comp.h"

class DamageComp : public Comp
{
public:
    enum class DamageOrigin 
    {
        player,
        enemies,
        neutral
    };

    DamageComp(DamageOrigin origin = DamageOrigin::neutral) 
    {
        damage = 0;
        this->origin = origin;
    };

    virtual ~DamageComp() {};

    static ComponentKey getStaticKey() { return ComponentKey::damage; };
    Comp* clone() const { return new DamageComp(*this); };

    DamageOrigin origin;
    int damage;
    sf::Vector2f knockback;
};