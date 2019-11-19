#include "FlyingAIComp.h"

FlyingAIComp::FlyingAIComp(sf::Vector2f pos, sf::Vector2f size):
    AIComp(pos, size)
{
    this->forcedDirResolved = false;
}

void FlyingAIComp::updateIdle(float dt, SpriteComp* sprite)
{
    this->movement.walkSpeed = idleSpeed;

    if (isDesicionTime() && this->forcedDirection == Direction::none)
    {
        int x = rand() % 2;
        int y = (rand() % 100) - 50;
        switch (x)
        {
        case 0:
            moveLeft(sprite);
            break;

        case 1:
            moveRight(sprite);
            break;
        default:
            this->movement.acceleration.x = 0;
            this->movement.acceleration.y = 0;
            break;
        }
        this->movement.acceleration.y = y / 50.f;
        normalize(this->movement.acceleration);
        forcedDirResolved = false;
        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        this->movement.acceleration = this->currentRoamPoint - this->collider.getCenterPos();
        normalize(this->movement.acceleration);
        forcedDirection = Direction::none;

        if (movement.acceleration.x > 0)
            faceRight(sprite);

        else
            faceLeft(sprite);

        forcedDirResolved = true;
        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->movement.transform.pos - this->currentRoamPoint;
    if (length(this->movement.transform.pos - this->currentRoamPoint) > this->roamDistance && !forcedDirResolved)
    {
        forcedDirection = Direction::right;
        forcedDirResolved = false;
    }
}

void FlyingAIComp::updateChasing(float dt, SpriteComp* sprite)
{
    this->movement.walkSpeed = chaseSpeed;

    if (lastKnownPlayerPos.x < collider.getCenterPos().x)
        faceLeft(sprite);

    else
        faceRight(sprite);

    int x = (rand() % 400) - 200;
    if (isDesicionTime())
    {
        x = (rand() % 400) - 200;

        desicionTimeOver();
    }


    this->movement.acceleration = lastKnownPlayerPos + sf::Vector2f(x, -200) - collider.getCenterPos();

    if (lengthSquared(movement.acceleration) > 1.f)
        normalize(movement.acceleration);

    if (attackCounter.update(dt) && lengthSquared(lastKnownPlayerPos - collider.getCenterPos()) < 500 * 500)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            movement.momentum.x = 0;
            movement.momentum.y = 0;
            attackCounter.reset();
            attackDestination = lastKnownPlayerPos + sf::Vector2f(0, -190);
            this->movement.acceleration = attackDestination - collider.getCenterPos();
            normalize(movement.acceleration);
            movement.acceleration *= 1.3f;
        }

        else
            state = State::returning;
    }

}

void FlyingAIComp::updateReturn(float dt, SpriteComp* sprite)
{
    this->movement.acceleration = this->currentRoamPoint - this->collider.getCenterPos();
    normalize(this->movement.acceleration);

    if (movement.acceleration.x > 0)
        faceRight(sprite);

    else
        faceLeft(sprite);

    if (lengthSquared(this->currentRoamPoint - collider.getCenterPos()) < 250)
        state = State::idle;
}

void FlyingAIComp::updateStunned(float dt, SpriteComp* sprite)
{
    this->movement.acceleration.x = 0;
    if (this->stunCounter.update(dt))
    {
        this->stunCounter.reset();

        if (!timeSincePlayerSeen.isTimeUp())
            this->state = State::chasing;

        else
        {
            this->state = State::searching;
            this->currentRoamPoint = lastKnownPlayerPos;
        }
    }
}

void FlyingAIComp::handleCollision(const Collidable* collidable, SpriteComp* sprite)
{
    if (collidable->getCollider().hasComponent(ColliderKeys::ground))
    {

        if (collidable->getCollider().intersects(collidable->getCollider().getLeftBox(), this->collider.getRightBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.acceleration.x *= -1;
            this->facingDir = Direction::left;
            this->movement.transform.pos.x = collidable->getCollider().left() - this->collider.width();
            this->movement.jump();

            if (sprite)
                sprite->flipHorizontally();
        }

        else if (collidable->getCollider().intersects(collidable->getCollider().getRightBox(), this->collider.getLeftBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.acceleration.x *= -1;
            this->facingDir = Direction::right;
            this->movement.transform.pos.x = collidable->getCollider().right();
            this->movement.jump();

            if (sprite)
                sprite->flipHorizontally();
        }


        //walking on ground
        else if (this->movement.momentum.y > 0 && collidable->getCollider().intersects(collidable->getCollider().getUpBox(), this->collider.getDownBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.acceleration.y *= -1;
            this->movement.transform.pos.y = collidable->getCollider().up() - this->collider.height();
            movement.grounded = true;
        }

        //smackin into roof
        else if (collidable->getCollider().intersects(collidable->getCollider().getDownBox(), this->collider.getUpBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.acceleration.y *= -1;
            this->movement.transform.pos.y = collidable->getCollider().down();
        }
    }

}
