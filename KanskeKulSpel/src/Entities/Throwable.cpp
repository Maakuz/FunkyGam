#include "Throwable.h"
#include "Misc/Definitions.h"
#include "Handlers/ParticleHandler.h"

//in MilSec
#define ARMING_TIME 500 

Throwable::Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture)
    :Entity(pos, texture)
{
    this->mass = 0.3f;
    this->momentum = momentum;

    this->armingCounter = 0;
    this->impacted = false;
    this->isArmed = false;
}

void Throwable::update(float dt)
{
    this->armingCounter += dt;
    if (this->armingCounter > ARMING_TIME)
        isArmed = true;

    momentum.y += GRAVITY * dt * this->mass;

    this->pos += momentum;

    this->updatePosition();
}

void Throwable::handleCollision(const Entity& collider)
{
    if (isArmed)
    {
        this->impacted = true;
        ParticleHandler::addEmitter(ParticleHandler::emitterTypes::bomb, this->pos + (this->size / 2.f));
    }
}
