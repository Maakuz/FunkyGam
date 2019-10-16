#include "Grunt.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"
#include <string>

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :Enemy(data, pos)
{
    this->roamDistance = 64;
    this->idleSpeed = 0.005;
    this->walkSpeed = idleSpeed;
    this->chaseSpeed = 0.01;
    this->state = State::idle;
    this->flying = false;

    this->boundReached = 0;
    this->attackDistance = 64;
    this->attackChargeTimer = Counter(1000);
    this->stunCounter = Counter(500);
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

    case Enemy::State::attacking:
        updateAttack(dt);
        break;

    case Enemy::State::returning:
        updateReturning(dt);
        break;

    case Enemy::State::stunned:
        if (this->stunCounter.update(dt))
        {
            this->stunCounter.reset();
            this->state = State::chasing;
            setAnimation(0);
        }
        break;
    }

    Enemy::update(dt);
}


void Grunt::updateIdle(float dt)
{
    if (isDesicionTime() && this->boundReached == 0)
    {
        int r = rand() % 3;
        printCon(std::to_string(r));
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
        printCon("End");
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
    if (timeSincePlayerSeen.isTimeUp()) //will sort of mitigating stuck enemies looking silly
        state = State::idle;

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


    if (lengthSquared(pos + eyeLevel - getLastKnownPos()) < this->attackDistance * this->attackDistance)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            this->attackChargeTimer.reset();
            setAnimation(1);
        }
        else
        {
            state = State::idle;
            setStartPoint(getLastKnownPos());
        }

    }
}

void Grunt::updateReturning(float dt)
{
}

void Grunt::updateAttack(float dt)
{
    this->acceleration.x = 0;
    if (attackChargeTimer.update(dt) && !flying)
    {
        if (facingDir == Direction::left)
            this->momentum = sf::Vector2f(-attackMomentum.x, -attackMomentum.y);

        else
            this->momentum = sf::Vector2f(attackMomentum.x, -attackMomentum.y);

        this->grounded = false;
        this->flying = true;
        setAnimation(2);
    }
}

std::istream& Grunt::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> chaseSpeed;
    in >> trash >> idleSpeed;
    in >> trash >> attackMomentum.x >> attackMomentum.y;
    in >> trash >> attackDistance;
    in >> trash >> attackChargeTimer.stopValue;
    in >> trash >> stunCounter.stopValue;

    return in;
}

void Grunt::handleCollision(const Entity& collider)
{
    if (flying)
    {
        this->flying = false;
        this->state = State::stunned;
        setAnimation(3);
    }

    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Platform))
    {
        if (this->momentum.y > 0 && collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.up() - this->height();
            grounded = true;
        }
    }

    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
    {
        //walking on ground
        if (this->momentum.y > 0 && collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.up() - this->height();
            grounded = true;
        }

        //smackin into roof
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.down();
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.left() - this->width();
            this->jump();
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.right();
            this->jump();
        }
    }
}