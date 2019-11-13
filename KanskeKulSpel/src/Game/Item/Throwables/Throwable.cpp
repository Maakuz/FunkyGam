#include "Throwable.h"
#include "Game/Misc/Definitions.h"
#include "Game/Particles/ParticleHandler.h"
#include "Game/Entities/MovingEntity.h"
#include <string>
#include "Misc/ConsoleWindow.h"

Throwable::Throwable(sf::Vector2f momentum, sf::Vector2f pos, const sf::Texture* texture)
    :Item(pos, texture)
{
    this->collider.addComponent(Collider::ColliderKeys::throwable);
    this->mass = 0.3f;
    this->armingCounter = 0;
    this->armingTime = 1000;
    this->momentum = momentum;
    this->bounce = 0.4f;
    this->armingCounter = 0;
    this->impacted = false;
    this->armed = false;
    this->detonateOnImpact = false;
    this->detonated = false;
    this->particleEffectID = 0;
    this->damage = 0;
    this->thrower = nullptr;
    this->addedMomentum = false;
}

void Throwable::update(float dt)
{
    if (addedMomentum)
    {
        this->momentum = collisionMomentum;

        addedMomentum = false;
    }

    this->armingCounter += dt;
    if (this->armingTime < this->armingCounter)
        armed = true;


    if (armed || (detonateOnImpact && impacted))
    {
        detonated = true;
        this->explosionData.center = getCenterPos();
        ParticleHandler::addEmitter(this->particleEffectID, this->pos + (this->getSize() / 2.f));
    }

    momentum.y += GRAVITY * dt * this->mass;

    this->pos += momentum;

    this->updateSpritePosition();
}

void Throwable::throwItem(sf::Vector2f pos, sf::Vector2f momentum, const Entity* thrower)
{
    this->setPosition(pos);
    this->momentum = momentum;
    this->thrower = thrower;
}

void Throwable::handleCollision(const Entity* colliderObj)
{
    //Projectile is immune to thrower NO, NOT THE OTHER WAY AROUND
    if (colliderObj == thrower || 
        colliderObj->getCollider().hasComponent(Collider::ColliderKeys::customTerrain) ||
        colliderObj->getCollider().hasComponent(Collider::ColliderKeys::levelReturn))
        return;

    if (colliderObj->getCollider().hasComponent(Collider::ColliderKeys::ground) ||
        colliderObj->getCollider().hasComponent(Collider::ColliderKeys::character))
    {
        if (colliderObj->getCollider().intersects(colliderObj->getCollider().getLeft(), this->collider.getAABB()))
        {
            this->momentum.x *= -bounce;
            this->pos.x = colliderObj->getPosition().x - this->getSize().x;
        }

        else if (colliderObj->getCollider().intersects(colliderObj->getCollider().getRight(), this->collider.getAABB()))
        {
            this->momentum.x *= -bounce;
            this->pos.x = colliderObj->getPosition().x + colliderObj->getSize().x;
        }

        else if (this->momentum.y > 0 && colliderObj->getCollider().intersects(colliderObj->getCollider().getUp(), this->collider.getAABB()))
        {
            this->momentum.y *= -bounce;
            this->momentum.x *= 0.96f;
            this->pos.y = colliderObj->getPosition().y - this->getSize().y;
        }

        else if (colliderObj->getCollider().intersects(colliderObj->getCollider().getDown(), this->collider.getAABB()))
        {
            this->momentum.y *= bounce;
            this->pos.y = colliderObj->getPosition().y + colliderObj->getSize().y;
        }
    }

    else if (colliderObj->getCollider().hasComponent(Collider::ColliderKeys::throwable))
    {
        const Throwable* ptr = dynamic_cast<const Throwable*>(colliderObj);

        this->addCollisionMomentum(ptr->getMomentum(), ptr->getCenterPos(), ptr->getMass());
    }

    this->impacted = true;
}

void Throwable::addCollisionMomentum(sf::Vector2f colliderMomentum, sf::Vector2f colliderPos, float colliderMass)
{
    this->collisionMomentum = Collider::calculateCollisionForceOnObject(this->getCenterPos(), colliderPos, this->momentum, colliderMomentum, this->mass, colliderMass);
    this->addedMomentum = true;
    this->pos += colliderMomentum;
}

std::istream& Throwable::readSpecific(std::istream& in)
{
    std::string trash;
    int limit;
    int emitterID;
    bool useable;


    in >> trash;
    in >> trash >> mass;
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
    out << "Mass: " << mass << "\n";
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
