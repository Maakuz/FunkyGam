#include "Throwable.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include <string>
#include "Misc/ConsoleWindow.h"

Throwable::Throwable(sf::Vector2f pos, const sf::Texture* texture, sf::Vector2f size)
    :Item(pos, texture),
    collider(size, pos)
{
    this->collider.addComponent(ColliderKeys::throwable);
    this->armingCounter = 0;
    this->armingTime = 1000;
    this->bounce = 0.4f;
    this->armingCounter = 0;
    this->impacted = false;
    this->armed = false;
    this->detonateOnImpact = false;
    this->detonated = false;
    this->particleEffectID = 0;
    this->damage = 0;
    this->thrower = nullptr;

    this->movement.setAirRes(1.f);
}

void Throwable::update(float dt)
{
    if (!detonateOnImpact)
        this->armingCounter += dt;

    if (this->armingTime < this->armingCounter)
        armed = true;


    if (armed || (detonateOnImpact && impacted))
    {
        detonated = true;
        this->explosionData.center = getTextureCenterPos();
        ParticleHandler::addEmitter(this->particleEffectID, this->movement.transform.pos + (this->collider.getSize() / 2.f));
    }

    movement.update(dt);
    this->sprite.setPosition(movement.transform.pos);
    this->collider.setPosition(movement.transform.pos);
}

void Throwable::throwItem(sf::Vector2f pos, sf::Vector2f momentum, const Collidable* thrower)
{
    this->movement.transform.pos = pos;
    this->movement.momentum = momentum;
    this->thrower = thrower;
}

void Throwable::handleCollision(const Collidable* collidable)
{
    //Projectile is immune to thrower NO, NOT THE OTHER WAY AROUND
    if (collidable == thrower ||
        collidable->getCollider().hasComponent(ColliderKeys::customTerrain) ||
        collidable->getCollider().hasComponent(ColliderKeys::levelReturn))
        return;

    if (collidable->getCollider().hasComponent(ColliderKeys::ground) ||
        collidable->getCollider().hasComponent(ColliderKeys::character))
    {
        if (collidable->getCollider().intersects(collidable->getCollider().getLeftBox(), this->collider.getAABB()))
        {
            this->movement.momentum.x *= -bounce;
            this->movement.transform.pos.x = (collidable->getCollider().left() - this->collider.width());
        }

        else if (collidable->getCollider().intersects(collidable->getCollider().getRightBox(), this->collider.getAABB()))
        {
            this->movement.momentum.x *= -bounce;
            this->movement.transform.pos.x = (collidable->getCollider().right());
        }

        else if (this->movement.momentum.y > 0 && collidable->getCollider().intersects(collidable->getCollider().getUpBox(), this->collider.getAABB()))
        {
            this->movement.momentum.y *= -bounce;
            this->movement.momentum.x *= 0.96f;
            this->movement.transform.pos.y = collidable->getCollider().up() - this->collider.height();
        }

        else if (collidable->getCollider().intersects(collidable->getCollider().getDownBox(), this->collider.getAABB()))
        {
            this->movement.momentum.y *= bounce;
            this->movement.transform.pos.y = (collidable->getCollider().down());
        }
    }

    else if (collidable->getCollider().hasComponent(ColliderKeys::throwable))
    {
        const Throwable* ptr = dynamic_cast<const Throwable*>(collidable);

        this->movement.addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider.getCenterPos(), ptr->collider.getCenterPos(), movement.momentum, ptr->movement.momentum, movement.mass, ptr->movement.mass));
    }

    this->impacted = true;
}

std::istream& Throwable::readSpecific(std::istream& in)
{
    std::string trash;
    int limit;
    int emitterID;
    bool useable;


    in >> trash;
    in >> trash >> movement.mass;
    in >> trash >> armingTime;
    in >> trash >> bounce;
    in >> trash >> detonateOnImpact;
    in >> trash >> damage;
    in >> trash >> particleEffectID;

    int explosionType;

    in >> trash;
    in >> trash >> explosionType;
    explosionData.type = ExplosionType(explosionType);
    in >> trash >> explosionData.radius;
    in >> trash >> explosionData.falloff;
    in >> trash >> explosionData.damage;

    return in;
}

std::ostream& Throwable::writeSpecific(std::ostream& out) const
{
    out << "[Specific]\n";
    out << "Mass: " << movement.mass << "\n";
    out << "ArmingTime: " << armingTime << "\n";
    out << "Bounce: "<< bounce << "\n";
    out << "DetonateOnImpact: "<< detonateOnImpact << "\n";
    out << "Damage: "<< damage << "\n";
    out << "ParticleEffectID: " << particleEffectID << "\n";


    out << "[Explosion]\n";
    out << "Type: " << (int)explosionData.type << "\n";
    out << "Radius: " << explosionData.radius << "\n";
    out << "Falloff: "<< explosionData.falloff << "\n";
    out << "Damage "<< explosionData.damage << "\n";

    return out;
}
