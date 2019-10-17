#pragma once
#include "SFML/System/Vector2.hpp"
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
};