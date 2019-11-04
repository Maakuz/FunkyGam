#include "Throwable.h"
#include "Game/Misc/Definitions.h"
#include "Game/Handlers/ParticleHandler.h"
#include <string>

Throwable::Throwable(sf::Vector2f momentum, sf::Vector2f pos, const sf::Texture* texture)
    :Item(pos, texture)
{
    this->collisionBox.addComponent(CollisionBox::ColliderKeys::throwable);
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
}

void Throwable::update(float dt)
{

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

void Throwable::handleCollision(const Entity* collider)
{
    //Projectile is immune to thrower NO, NOT THE OTHER WAY AROUND
    if (collider == thrower || 
        collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::customTerrain) ||
        collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::levelReturn))
        return;

    if (this->momentum.y > 0 && collider->getCollisionBox().intersects(collider->getCollisionBox().getUp(), this->collisionBox.getDown()))
    {
        this->momentum.y *= -bounce;
        this->momentum.x *= 0.96f;
        this->pos.y = collider->getPosition().y - this->getSize().y;
    }

    if (collider->getCollisionBox().intersects(collider->getCollisionBox().getDown(), this->collisionBox.getUp()))
    {
        this->momentum.y *= bounce;
        this->pos.y = collider->getPosition().y + collider->getSize().y;
    }

    if (collider->getCollisionBox().intersects(collider->getCollisionBox().getLeft(), this->collisionBox.getRight()))
    {
        this->momentum.x *= -bounce;
        this->pos.x = collider->getPosition().x - this->getSize().x;
    }

    if (collider->getCollisionBox().intersects(collider->getCollisionBox().getRight(), this->collisionBox.getLeft()))
    {
        this->momentum.x *= -bounce;
        this->pos.x = collider->getPosition().x + collider->getSize().x;
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
