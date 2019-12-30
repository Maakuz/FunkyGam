#include "Throwable.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include <string>
#include "Misc/ConsoleWindow.h"
#include "Game/Components/LogisticsComp.h"

Throwable::Throwable(sf::Vector2f pos, const sf::Texture* texture, sf::Vector2f size):
    Collidable(pos, size)
{
    this->getComponent<ColliderComp>()->addComponent(ColliderKeys::throwable);
    this->armingCounter = 0;
    this->armingTime = 1000;
    this->bounce = 0.4f;
    this->armingCounter = 0;
    this->impacted = false;
    this->armed = false;
    this->detonateOnImpact = false;
    this->detonated = false;
    this->particleEffectID = 0;

    MovementComp* movement = new MovementComp;
    addComponent<MovementComp>(movement);

    SpriteComp* sprite = new SpriteComp(texture, pos);
    addComponent<SpriteComp>(sprite);

    LogisticsComp* logistics = new LogisticsComp;
    addComponent<LogisticsComp>(logistics);

    addComponent<DamageComp>(new DamageComp);

    movement->setAirRes(1.f);
}

void Throwable::update(float dt)
{
    MovementComp* movement = getComponent<MovementComp>();
    ColliderComp* collider = getComponent<ColliderComp>();
    SpriteComp* sprite = getComponent<SpriteComp>();

    if (!detonateOnImpact)
        this->armingCounter += dt;

    if (this->armingTime < this->armingCounter)
        armed = true;


    if (armed || (detonateOnImpact && impacted))
    {
        detonated = true;
        this->explosionData.center = collider->getCenterPos();
        ParticleHandler::addEmitter(this->particleEffectID, movement->transform.pos + (collider->getSize() / 2.f));
    }

    movement->update(dt);
    sprite->setPosition(movement->transform.pos);
    collider->setPosition(movement->transform.pos);
}

void Throwable::throwItem(sf::Vector2f pos, sf::Vector2f momentum, DamageComp::DamageOrigin origin)
{
    MovementComp* movement = getComponent<MovementComp>();

    movement->transform.pos = pos;
    getColliderComp()->setPosition(pos);
    movement->momentum = momentum;
    this->getComponent<DamageComp>()->origin = origin;
}

void Throwable::handleCollision(const Collidable* collidable)
{
    const ColliderComp* otherCollider = collidable->getColliderComp();
    const ColliderComp* collider = getColliderComp();
    MovementComp* movement = getMovementComp();
    const DamageComp::DamageOrigin* origin = &getDamageComp()->origin;

    if ((otherCollider->hasComponent(ColliderKeys::player) && *origin == DamageComp::DamageOrigin::player) ||
        (otherCollider->hasComponent(ColliderKeys::enemy) && *origin == DamageComp::DamageOrigin::enemies) ||
        otherCollider->hasComponent(ColliderKeys::customTerrain) ||
        otherCollider->hasComponent(ColliderKeys::levelReturn))
        return;

    if (otherCollider->hasComponent(ColliderKeys::ground) ||
        otherCollider->hasComponent(ColliderKeys::enemy) || 
        otherCollider->hasComponent(ColliderKeys::player))
    {
        if (otherCollider->intersects(otherCollider->getLeftBox(), collider->getAABB()))
        {
            movement->momentum.x *= -bounce;
            movement->transform.pos.x = (otherCollider->left() - collider->width());
        }

        else if (otherCollider->intersects(otherCollider->getRightBox(), collider->getAABB()))
        {
            movement->momentum.x *= -bounce;
            movement->transform.pos.x = (otherCollider->right());
        }

        else if (movement->momentum.y > 0 && otherCollider->intersects(otherCollider->getUpBox(), collider->getAABB()))
        {
            movement->momentum.y *= -bounce;
            movement->momentum.x *= 0.96f;
            movement->transform.pos.y = otherCollider->up() - collider->height();
        }

        else if (otherCollider->intersects(otherCollider->getDownBox(), collider->getAABB()))
        {
            movement->momentum.y *= bounce;
            movement->transform.pos.y = (otherCollider->down());
        }
    }

    else if (otherCollider->hasComponent(ColliderKeys::throwable))
    {
        const MovementComp* otherMovement = collidable->getMovementComp();
        movement->addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider->getCenterPos(), otherCollider->getCenterPos(), movement->momentum, otherMovement->momentum, movement->mass, otherMovement->mass));
    }

    this->impacted = true;
}

void Throwable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*getSpriteComp(), states);
}

std::istream& operator>>(std::istream& in, Throwable& throwable)
{
    std::string trash;
    int limit;
    int emitterID;
    bool useable;

    in >> *throwable.getComponent<LogisticsComp>();

    in >> trash;
    in >> trash >> throwable.getComponent<MovementComp>()->mass;
    in >> trash >> throwable.armingTime;
    in >> trash >> throwable.bounce;
    in >> trash >> throwable.detonateOnImpact;
    in >> trash >> throwable.getComponent<DamageComp>()->damage;
    in >> trash >> throwable.particleEffectID;

    int explosionType;

    in >> trash;
    in >> trash >> explosionType;
    throwable.explosionData.type = ExplosionType(explosionType);
    in >> trash >> throwable.explosionData.radius;
    in >> trash >> throwable.explosionData.falloff;
    in >> trash >> throwable.explosionData.damage;

    return in;
}

std::ostream& operator<<(std::ostream& out, const Throwable& throwable)
{
    out << *throwable.getComponent<LogisticsComp>();

    out << "[Specific]\n";
    out << "Mass: " << throwable.getComponent<MovementComp>()->mass << "\n";
    out << "ArmingTime: " << throwable.armingTime << "\n";
    out << "Bounce: " << throwable.bounce << "\n";
    out << "DetonateOnImpact: " << throwable.detonateOnImpact << "\n";
    out << "Damage: " << throwable.getComponent<DamageComp>()->damage << "\n";
    out << "ParticleEffectID: " << throwable.particleEffectID << "\n";


    out << "[Explosion]\n";
    out << "Type: " << (int)throwable.explosionData.type << "\n";
    out << "Radius: " << throwable.explosionData.radius << "\n";
    out << "Falloff: " << throwable.explosionData.falloff << "\n";
    out << "Damage " << throwable.explosionData.damage << "\n";

    return out;
}
