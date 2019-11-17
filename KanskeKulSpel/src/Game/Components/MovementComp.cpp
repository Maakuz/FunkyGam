#include "MovementComp.h"
#define GRAVITY 0.1f


MovementComp::MovementComp()
{
    this->walkSpeed = 0.f;
    this->jumpHeight = 0.f;

    this->mass = 0.f;
    this->grounded = false;
    this->addedMomentum = false;
    this->jumping = false;
    this->ignoringGravity = false;

    this->floorRes = 0.85f;
    this->airRes = 0.985f;
    this->airMobility = 0.1f;
}

void MovementComp::reset()
{
    this->acceleration = sf::Vector2f(0, 0);
    this->momentum = sf::Vector2f(0, 0);
}

sf::Vector2f MovementComp::update(float dt, sf::Vector2f pos)
{
    if (addedMomentum)
    {
        this->momentum = collisionMomentum;

        addedMomentum = false;
    }

    if (momentum.y > 0)
    {
        if (this->jumping)
            jumping = false;

        grounded = false;
    }

    if (!ignoringGravity)
        momentum.y += GRAVITY * dt * this->mass;

    if (grounded)
    {
        momentum += acceleration * walkSpeed * dt;
        momentum *= floorRes;
    }
    else
    {
        momentum += acceleration * walkSpeed * dt * airMobility;
        momentum *= airRes;
    }



    pos += momentum;

    return pos;
}

void MovementComp::jump()
{
    if (this->grounded)
    {
        momentum.y = -jumpHeight;
        this->grounded = false;
        this->jumping = true;
    }
}

void MovementComp::stopJump(float haltForce)
{
    if (this->jumping)
    {
        this->jumping = false;
        this->momentum.y *= haltForce;
    }
}

void MovementComp::addCollisionMomentum(sf::Vector2f momentum)
{
    this->collisionMomentum = momentum;
    this->addedMomentum = true;
}
