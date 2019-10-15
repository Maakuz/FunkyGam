#include "Throwable.h"
#include "Misc/Definitions.h"
#include "Handlers/ParticleHandler.h"

#define IMPACT_DELAY 50

Throwable::Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture)
    :Entity(pos, texture)
{
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
    this->collisionDelayTimer = 0;
}

void Throwable::update(float dt)
{
    this->collisionDelayTimer += dt;

    this->armingCounter += dt;
    if (this->armingTime < this->armingCounter)
        armed = true;


    if (armed || (detonateOnImpact && impacted))
    {
        detonated = true;
        ParticleHandler::addEmitter(ParticleHandler::emitterTypes::bomb, this->pos + (this->size / 2.f));
    }

    momentum.y += GRAVITY * dt * this->mass;

    this->pos += momentum;

    this->updatePosition();
}

void Throwable::handleCollision(const Entity& collider)
{
    if (collisionDelayTimer < IMPACT_DELAY)
        return;

    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
    {
        if (this->momentum.y > 0 && collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y *= -bounce;
            this->momentum.x *= 0.96;
            this->pos.y = collider.getPosition().y - this->size.y;
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y *= bounce;
            this->pos.y = collider.getPosition().y + collider.getSize().y;
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -bounce;
            this->pos.x = collider.getPosition().x - this->size.x;
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -bounce;
            this->pos.x = collider.getPosition().x + collider.getSize().x;
        }
    }

    
    this->impacted = true;
}
