#include "Throwable.h"
#include "Misc/Definitions.h"

Throwable::Throwable(sf::Vector2f momentum, sf::Vector2f pos, sf::Texture* texture)
    :Entity(pos, texture)
{
    this->mass = 0.3;
    this->momentum = momentum;
}

void Throwable::update(float dt)
{
    momentum.y += GRAVITY * dt * this->mass;

    this->pos += momentum;

    this->updatePosition();
}

void Throwable::handleCollision(const Entity& collider)
{

}
