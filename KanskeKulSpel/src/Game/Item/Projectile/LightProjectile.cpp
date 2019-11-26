#include "LightProjectile.h"

LightProjectile::LightProjectile() :
    collider(sf::Vector2f(), sf::Vector2f())
{
    movement.setIgnoreGravity(true);
    movement.setAirRes(1);
}

void LightProjectile::update(float dt)
{
}

void LightProjectile::shoot(sf::Vector2f pos, sf::Vector2f dir, Collidable* owner)
{
}

void LightProjectile::handleCollision(const Collidable* collidable)
{
}

std::istream& operator>>(std::istream& in, LightProjectile& projectile)
{
    return in;
}

std::ostream& operator<<(std::ostream& out, const LightProjectile& projectile)
{
    return out;
}
