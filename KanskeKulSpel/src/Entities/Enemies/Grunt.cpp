#include "Grunt.h"
#include "Misc/VectorFunctions.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :Enemy(data, pos)
{
    this->roamDistance = 64;
    this->idleSpeed = 0.005;
    this->walkSpeed = idleSpeed;
    this->chaseSpeed = 0.01;
    this->state = State::idle;

    this->boundReached = 0;
}

void Grunt::update(float dt)
{

    switch (state)
    {
    case Enemy::State::idle:
        this->walkSpeed = idleSpeed;
        updateIdle(dt);
        break;

    case Enemy::State::chasing:
        this->walkSpeed = chaseSpeed;
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
    if (isDesicionTime() && this->boundReached == 0)
    {
        int r = rand() % 3;
        printf("%d\n", r);
        switch (r)
        {
        case 0:
            this->acceleration.x = 0;
            break;

        case 1:
            this->acceleration.x = -1;
            this->facingDir = Direction::left;
            if (!this->isFlippedHorizontally())
                this->flipHorizontally();
            break;

        case 2:
            this->acceleration.x = 1;
            if (this->isFlippedHorizontally())
                this->flipHorizontally();
            this->facingDir = Direction::right;
            break;

        default:
            this->acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        printf("End\n");
        if (this->facingDir == Direction::left)
            this->facingDir = Direction::right;

        else
            this->facingDir = Direction::left;

        this->acceleration.x = -boundReached;
        this->boundReached = 0;
        this->flipHorizontally();
        this->desicionTimeOver();
    }

    if (length(this->pos - this->getStartPoint()) > this->roamDistance && this->boundReached == 0)
    {
        this->boundReached = acceleration.x;
        this->acceleration.x = 0;
        this->momentum.x *= -1;
        this->desicionTimeOver();
    }

}

void Grunt::updateChasing(float dt)
{
    printf("!");
    if (this->getLastKnownPos().x < this->pos.x)
    {
        this->facingDir = Direction::left;
        if (!this->isFlippedHorizontally())
            this->flipHorizontally();
        this->acceleration.x = -1;
    }

    else if (this->getLastKnownPos().x >= this->pos.x)
    {
        if (this->isFlippedHorizontally())
            this->flipHorizontally();
        this->facingDir = Direction::right;
        this->acceleration.x = 1;
    }
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