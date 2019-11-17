#pragma once
#include <istream>
#include <ostream>
#include <algorithm>
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

    friend std::istream& operator>>(std::istream& in, Explosion& explosion)
    {
        std::string trash;
        int type;
        in >> trash;
        in >> trash >> explosion.damage;
        in >> trash >> explosion.radius;
        in >> trash >> explosion.falloff;
        in >> trash >> type;
        explosion.type = ExplosionType(type);

        return in;
    }

    friend std::ostream& operator<<(std::ostream& out, const Explosion& explosion)
    {
        out << "[Explosion]" << "\n";
        out << "Damage: " << explosion.damage << "\n";
        out << "Radius: " << explosion.radius << "\n";
        out << "Falloff: " << explosion.falloff << "\n";
        out << "Type: " << (int)explosion.type << "\n";
        return out;
    }

};