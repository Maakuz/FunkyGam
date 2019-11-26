#include "LightProjectile.h"
#include "Game/Particles/ParticleHandler.h"

LightProjectile::LightProjectile() :
    collider(sf::Vector2f(), sf::Vector2f())
{
    this->destroyed = false;
    this->light = nullptr;
    this->owner = nullptr;
    this->initialEmitterID = 0;
    this->lightEmitterID = 0;
    this->damage = 0;
    this->velocity = 0;

    movement.setIgnoreGravity(true);
    movement.setAirRes(1);
}

LightProjectile::~LightProjectile()
{
    if (this->light)
        this->light->kill();
}

void LightProjectile::update(float dt)
{
    this->movement.update(dt);
    this->collider.setPosition(movement.transform.pos);
    this->light->setEmitterPos(collider.getCenterPos());
}

void LightProjectile::shoot(sf::Vector2f pos, sf::Vector2f dir, Collidable* owner)
{
    this->movement.transform.pos = pos;
    this->movement.momentum = dir * this->velocity;
    this->owner = owner;
    this->light = ParticleHandler::addEmitter(lightEmitterID, collider.getCenterPos());
    ParticleHandler::addEmitter(initialEmitterID, collider.getCenterPos());
}

void LightProjectile::handleCollision(const Collidable* collidable)
{
    if (collidable == owner)
        return;

    this->destroyed = true;
}

std::istream& operator>>(std::istream& in, LightProjectile& projectile)
{
    float size;
    in >> projectile.lightEmitterID;
    in >> projectile.initialEmitterID;
    in >> projectile.damage;
    in >> projectile.velocity;
    in >> size;
    projectile.collider.setSize(sf::Vector2f(size, size));

    return in;
}

std::ostream& operator<<(std::ostream& out, const LightProjectile& projectile)
{
    out << projectile.lightEmitterID << "\n";
    out << projectile.initialEmitterID << "\n";
    out << projectile.damage << "\n";
    out << projectile.velocity << "\n";
    out << projectile.getCollider().getSize().x << "\n";
    return out;
}
