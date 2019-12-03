#include "GroundAIComp.h"
#include "Game/Misc/VectorFunctions.h"

GroundAIComp::GroundAIComp(sf::Vector2f pos, sf::Vector2f size):
    AIComp(pos, size)
{
    attackDistance = 0;
}

void GroundAIComp::updateIdle(float dt, SpriteComp* sprite)
{
    this->movement.walkSpeed = idleSpeed;

    if (isDesicionTime() && this->forcedDirection == Direction::none)
    {
        int r = rand() % 3;
        switch (r)
        {
        case 0:
            this->movement.acceleration.x = 0;
            break;

        case 1:
            moveLeft(sprite);
            break;

        case 2:
            moveRight(sprite);
            break;

        default:
            this->movement.acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        if (this->forcedDirection == Direction::right)
            moveRight(sprite);

        else
            moveLeft(sprite);

        this->forcedDirection = Direction::none;

        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->movement.transform.pos - this->currentRoamPoint;
    if (length(this->movement.transform.pos - this->currentRoamPoint) > this->roamDistance)
    {
        if (roampointToPos.x > 0)
            forcedDirection = Direction::left;

        else
            forcedDirection = Direction::right;
    }


}

void GroundAIComp::updateChasing(float dt, SpriteComp* sprite)
{
    this->movement.walkSpeed = chaseSpeed;

    if (timeSincePlayerSeen.isTimeUp()) //will sort of mitigating stuck enemies looking silly
    {
        state = State::searching;
        this->currentRoamPoint = lastKnownPlayerPos;
        this->searchCounter.reset();
    }

    if (this->lastKnownPlayerPos.x < this->movement.transform.pos.x)
    {
        moveLeft(sprite);
    }

    else if (this->lastKnownPlayerPos.x >= this->movement.transform.pos.x)
    {
        moveRight(sprite);
    }


    if (lengthSquared(this->movement.transform.pos + eyeLevel - lastKnownPlayerPos) < this->attackDistance * this->attackDistance)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            this->attackChargeTimer.reset();
        }
    }

    if (timeSincePlayerSeen > 200 && abs(movement.transform.pos.x - lastKnownPlayerPos.x) < 10)
    {
        state = State::searching;
        this->currentRoamPoint = lastKnownPlayerPos;
        this->searchCounter.reset();
    }
}

void GroundAIComp::updateReturn(float dt, SpriteComp* sprite)
{
    if (this->currentRoamPoint.x < this->movement.transform.pos.x)
    {
        moveLeft(sprite);
    }

    else if (this->currentRoamPoint.x >= this->movement.transform.pos.x)
    {
        moveRight(sprite);
    }

    if (abs(movement.transform.pos.x - currentRoamPoint.x) < 10)
    {
        state = State::idle;
    }
}

void GroundAIComp::updateSearch(float dt, SpriteComp* sprite)
{
    updateIdle(dt);

    if (searchCounter.update(dt))
    {
        this->currentRoamPoint = getStartPoint();
        this->state = State::returning;
    }
}

void GroundAIComp::updateStunned(float dt, SpriteComp* sprite)
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
            this->searchCounter.reset();
        }
    }
}

void GroundAIComp::handleCollision(const ColliderComp* otherCollider)
{
    if (otherCollider->hasComponent(ColliderKeys::ground) || otherCollider->hasComponent(ColliderKeys::platform))
    {
        //walking on ground
        if (this->movement.momentum.y > 0 && ColliderComp::intersects(otherCollider->getUpBox(), this->collider.getDownBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.transform.pos.y = otherCollider->up() - this->collider.height();
            movement.grounded = true;
        }

        //smackin into roof
        if (ColliderComp::intersects(otherCollider->getDownBox(), this->collider.getUpBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.transform.pos.y = otherCollider->down();
        }

        if (ColliderComp::intersects(otherCollider->getLeftBox(), this->collider.getRightBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.transform.pos.x = otherCollider->left() - this->collider.width();
            this->movement.jump();
        }

        if (ColliderComp::intersects(otherCollider->getRightBox(), this->collider.getLeftBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.transform.pos.x = otherCollider->right();
            this->movement.jump();
        }
    }
}
