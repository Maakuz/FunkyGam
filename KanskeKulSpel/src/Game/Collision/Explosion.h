#pragma once
#include "SFML/System/Vector2.hpp"
#include "Game/Misc/VectorFunctions.h"
enum class ExplosionType 
{
    damage,
    flash,
    sound
};
struct Explosion
{
    sf::Vector2f center;
    float radius;
    int damage;
    float falloff;
    ExplosionType type;

    Explosion(sf::Vector2f center = sf::Vector2f(), float radius = 0, ExplosionType type = ExplosionType::damage, int damage = 0, float falloff = 0)
    {
        this->radius = radius;
        this->center = center;
        this->damage = damage;
        this->falloff = falloff;
        this->type = type;
    }

    float calculateDamage(sf::Vector2f entityPos) const
    {
        return damage - (damage * std::min(std::max((length(center - entityPos) / radius) - falloff, 0.f), 1.f));
    }
};