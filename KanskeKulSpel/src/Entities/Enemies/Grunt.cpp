#include "Grunt.h"
#include "Misc/VectorFunctions.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :Enemy(data, pos)
{
    this->roamDistance = 64;
    this->walkSpeed = 0.005;
    this->state = State::idle;

    this->boundReached = 0;
}

void Grunt::update(float dt)
{
    switch (state)
    {
    case Enemy::State::idle:
        updateIdle(dt);
        break;

    case Enemy::State::chasing:
        updateChasing(dt);
        break;

    case Enemy::State::returning:
        updateReturning(dt);
        break;
    }

    Enemy::update(dt);
}


void Grunt::updateIdle(float dt)
{
    if (isDesicionTime() && boundReached == 0)
    {
        int r = rand() % 3;
        printf("%d\n", r);
        switch (r)
        {
        case 0:
            acceleration.x = 0;
            break;

        case 1:
            acceleration.x = -1;
            if (!this->isFlippedHorizontally())
                this->flipHorizontally();
            break;

        case 2:
            acceleration.x = 1;
            if (this->isFlippedHorizontally())
                this->flipHorizontally();
            break;

        default:
            acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        acceleration.x = -boundReached;
        boundReached = 0;
        this->flipHorizontally();
        desicionTimeOver();
    }

    if (length(this->pos - this->getStartPoint()) > this->roamDistance)
    {
        this->boundReached = acceleration.x;
        acceleration.x = 0;
        momentum.x *= -1;
        desicionTimeOver();
    }

    //Search for player

}

void Grunt::updateChasing(float dt)
{
}

void Grunt::updateReturning(float dt)
{
}

void Grunt::handleCollision(const Entity& collider)
{
    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Platform))
    {
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.getPosition().y - this->size.y;
            grounded = true;
        }
    }

    MovingEntity::handleCollision(collider);
}