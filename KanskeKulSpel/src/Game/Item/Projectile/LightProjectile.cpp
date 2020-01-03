#include "LightProjectile.h"
#include "Game/Particles/ParticleHandler.h"

LightProjectile::LightProjectile()
{
    this->destroyed = false;
    this->light = nullptr;
    this->lifeTime = 0;
    this->impactEmitterID = 0;
    this->initialEmitterID = 0;
    this->lightEmitterID = 0;
    this->velocity = 0;
    this->getComponent<ColliderComp>()->addComponent(ColliderKeys::projectile);

    MovementComp* movement = new MovementComp;
    this->addComponent<MovementComp>(movement);

    addComponent<DamageComp>(new DamageComp);

    movement->setIgnoreGravity(true);
    movement->setAirRes(1);
}

LightProjectile::~LightProjectile()
{
    if (this->light)
        ParticleHandler::destroyEmitter(this->light);
}

void LightProjectile::update(float dt)
{
    ColliderComp* collider = getComponent<ColliderComp>();
    MovementComp* movement = getComponent<MovementComp>();
    movement->update(dt);
    collider->setPosition(movement->transform.pos);

    lifeTime += dt;
    if (lifeTime > 20000)
        destroyed = true;

    if (this->light)
    {
        this->light->setEmitterPos(collider->getCenterPos());
        ParticleHandler::queueEmitter(this->light);
    }
}

void LightProjectile::shoot(sf::Vector2f pos, sf::Vector2f dir, DamageComp::DamageOrigin origin)
{
    MovementComp* movement = getComponent<MovementComp>();
    ColliderComp* collider = getComponent<ColliderComp>();

    getDamageComp()->origin = origin;

    movement->transform.pos = pos;
    collider->setPosition(pos);
    movement->momentum = dir * this->velocity;
    this->light = ParticleHandler::createEmitter(lightEmitterID, collider->getCenterPos());

    if (initialEmitterID != -1)
        ParticleHandler::addEmitter(initialEmitterID, collider->getCenterPos());
}

void LightProjectile::handleCollision(const Collidable* collidable)
{
    const ColliderComp* otherCollider = collidable->getColliderComp();
    const DamageComp::DamageOrigin* origin = &getDamageComp()->origin;

    if ((otherCollider->hasComponent(ColliderKeys::player) && *origin == DamageComp::DamageOrigin::player) ||
        (otherCollider->hasComponent(ColliderKeys::enemy) && *origin == DamageComp::DamageOrigin::enemies) ||
        otherCollider->hasComponent(ColliderKeys::customTerrain) ||
        otherCollider->hasComponent(ColliderKeys::levelReturn) ||
        otherCollider->hasComponent(ColliderKeys::projectilePassable) ||
        otherCollider->hasComponent(ColliderKeys::platform))
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
    in >> projectile.getDamageComp()->damage;
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
    out << projectile.getDamageComp()->damage << "\n";
    out << projectile.velocity << "\n";
    out << projectile.getComponent<ColliderComp>()->getSize().x << "\n";
    return out;
}
