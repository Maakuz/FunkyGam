#pragma once
#include "SFML/System/Vector2.hpp"
#include "Game/Misc/VectorFunctions.h"
enum ExplosionType 
{
    basic = 0,
    flash = 1,
    bird = 2
};

namespace ExplosionStrings
{
    const int TYPE_COUNT = 3;
    const std::string types[TYPE_COUNT] = { "Basic", "Flash", "bird" };
}

struct Explosion
{
    sf::Vector2f center;
    float radius;
    int damage;
    float falloff;
    ExplosionType type;

    Explosion(sf::Vector2f center = sf::Vector2f(), float radius = 0, ExplosionType type = ExplosionType::basic, int damage = 0, float falloff = 0)
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