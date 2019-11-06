#include "MovingEntity.h"
#include "Game/Misc/Definitions.h"
#include "Game/Misc/VectorFunctions.h"

MovingEntity::MovingEntity(AnimationData data, sf::Vector2f pos)
    :AnimatedEntity(data, pos)
{
    this->walkSpeed = 0.05f;
    this->floorRes = 0.85f;
    this->airRes = 0.985f;
    this->airMobility = 0.1f;
    this->jumpHeight = 5.3f;
    this->mass = 0.166f;
    this->grounded = false;
    this->addedMomentum = false;
    this->jumping = false;
}

void MovingEntity::update(float dt)
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


    if (grounded)
    {
        momentum.x += acceleration.x * walkSpeed * dt;
        momentum.x *= floorRes;
    }
    else
    {
        momentum.x += acceleration.x * walkSpeed * dt * airMobility;
        momentum.x *= airRes;
    }

    momentum.y += GRAVITY * dt * this->mass;
    momentum.y *= airRes;

    this->pos += momentum;
    
    this->updateSpritePosition();

    if (abs(momentum.x) < this->walkSpeed * 0.75f && !isIdle())
        pauseAnimation();

    else if (abs(momentum.x) > this->walkSpeed * 0.75f)
        resumeAnimation();

    this->updateAnimation(dt);
}


void MovingEntity::jump()
{
    if (this->grounded)
    {
        momentum.y = -jumpHeight;
        this->grounded = false;
        this->jumping = true;
    }
}

void MovingEntity::stopJump(float haltForce)
{
    if (this->jumping)
    {
        this->jumping = false;
        this->momentum.y *= haltForce;
    }
}


void MovingEntity::handleCollision(const Entity* collider)
{
    if (collider->getCollider().hasComponent(Collider::ColliderKeys::ground))
    {
        //walking on ground
        if (this->momentum.y > 0 && collider->getCollider().intersects(collider->getCollider().getUp(), this->collider.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->up() - this->height();
            grounded = true;
        }

        //smackin into roof
        if (this->momentum.y < 0 && collider->getCollider().intersects(collider->getCollider().getDown(), this->collider.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->down();
        }

        if (collider->getCollider().intersects(collider->getCollider().getLeft(), this->collider.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->left() - this->width();
        }

        if (collider->getCollider().intersects(collider->getCollider().getRight(), this->collider.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->right();
        }
    }

    
}

void MovingEntity::addCollisionMomentum(sf::Vector2f colliderMomentum, sf::Vector2f colliderPos, float colliderMass)
{
    this->collisionMomentum = Collider::calculateCollisionForceOnObject(this->getCenterPos(), colliderPos, this->momentum, colliderMomentum, this->mass, colliderMass);
    this->addedMomentum = true;
}
