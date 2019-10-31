#include "MovingEntity.h"
#include "Misc/Definitions.h"
#include "Misc/VectorFunctions.h"

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
    if (collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::ground))
    {
        //walking on ground
        if (this->momentum.y > 0 && collider->getCollisionBox().intersects(collider->getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->up() - this->height();
            grounded = true;
        }

        //smackin into roof
        if (collider->getCollisionBox().intersects(collider->getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->down();
        }

        if (collider->getCollisionBox().intersects(collider->getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->left() - this->width();
        }

        if (collider->getCollisionBox().intersects(collider->getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->right();
        }
    }

    
}

void MovingEntity::addCollisionMomentum(sf::Vector2f colliderMomentum, sf::Vector2f colliderPos, float colliderMass)
{
    float v1 = length(this->momentum);
    float v2 = length(colliderMomentum);
    float v1new = ((this->mass - colliderMass) / (this->mass + colliderMass) * v1) + (2 * colliderMass / (this->mass + colliderMass) * v2);
    sf::Vector2f dir = this->pos - colliderPos;
    normalize(dir);
    dir.x *= v1new;
    dir.y *= v1new;
    this->collisionMomentum = dir;
    this->addedMomentum = true;
}
