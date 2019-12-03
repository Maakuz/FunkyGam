#include "LightProjectile.h"
#include "Game/Particles/ParticleHandler.h"

LightProjectile::LightProjectile()
{
    this->destroyed = false;
    this->light = nullptr;
    this->owner = nullptr;
    this->impactEmitterID = 0;
    this->initialEmitterID = 0;
    this->lightEmitterID = 0;
    this->damage = 0;
    this->velocity = 0;
    this->getComponent<ColliderComp>()->addComponent(ColliderKeys::projectile);
    MovementComp* movement = new MovementComp;
    this->addComponent(movement);
    movement->setIgnoreGravity(true);
    movement->setAirRes(1);
}

LightProjectile::~LightProjectile()
{
    if (this->light)
        this->light->killQuick();
}

void LightProjectile::update(float dt)
{
    ColliderComp* collider = getComponent<ColliderComp>();
    MovementComp* movement = getComponent<MovementComp>();
    movement->update(dt);
    collider->setPosition(movement->transform.pos);

    if (this->light)
        this->light->setEmitterPos(collider->getCenterPos());
}

void LightProjectile::shoot(sf::Vector2f pos, sf::Vector2f dir, Collidable* owner)
{
    MovementComp* movement = getComponent<MovementComp>();
    ColliderComp* collider = getComponent<ColliderComp>();

    movement->transform.pos = pos;
    collider->setPosition(pos);
    movement->momentum = dir * this->velocity;
    this->owner = owner;
    this->light = ParticleHandler::addEmitter(lightEmitterID, collider->getCenterPos());

    if (initialEmitterID != -1)
        ParticleHandler::addEmitter(initialEmitterID, collider->getCenterPos());
}

void LightProjectile::handleCollision(const Collidable* collidable)
{
    if (collidable == owner || collidable->getComponent<ColliderComp>()->hasComponent(ColliderKeys::projectilePassable))
        return;

    this->destroyed = true;

    if (impactEmitterID != -1)
        ParticleHandler::addEmitter(impactEmitterID, getComponent<ColliderComp>()->getCenterPos());
}

std::istream& operator>>(std::istream& in, LightProjectile& projectile)
{
    float size;
    in >> projectile.lightEmitterID;
    in >> projectile.initialEmitterID;
    in >> projectile.impactEmitterID;
    in >> projectile.damage;
    in >> projectile.velocity;
    in >> size;
    projectile.getComponent<ColliderComp>()->setSize(sf::Vector2f(size, size));

    return in;
}

std::ostream& operator<<(std::ostream& out, const LightProjectile& projectile)
{
    out << projectile.lightEmitterID << "\n";
    out << projectile.initialEmitterID << "\n";
    out << projectile.impactEmitterID << "\n";
    out << projectile.damage << "\n";
    out << projectile.velocity << "\n";
    out << projectile.getComponent<ColliderComp>()->getSize().x << "\n";
    return out;
}
