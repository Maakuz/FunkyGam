#include "GroundAIComp.h"
#include "Game/Misc/VectorFunctions.h"

GroundAIComp::GroundAIComp(sf::Vector2f pos, sf::Vector2f size):
    AIComp(pos, size)
{
    attackDistance = 0;
}

void GroundAIComp::updateIdle(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    movement->walkSpeed = idleSpeed;

    if (isDesicionTime() && this->forcedDirection == Direction::none)
    {
        int r = rand() % 3;
        switch (r)
        {
        case 0:
            movement->acceleration.x = 0;
            break;

        case 1:
            moveLeft(movement, sprite);
            break;

        case 2:
            moveRight(movement, sprite);
            break;

        default:
            movement->acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        if (this->forcedDirection == Direction::right)
            moveRight(movement, sprite);

        else
            moveLeft(movement, sprite);

        this->forcedDirection = Direction::none;

        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = movement->transform.pos - this->currentRoamPoint;
    if (length(movement->transform.pos - this->currentRoamPoint) > this->roamDistance)
    {
        if (roampointToPos.x > 0)
            forcedDirection = Direction::left;

        else
            forcedDirection = Direction::right;
    }


}

void GroundAIComp::updateChasing(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    movement->walkSpeed = chaseSpeed;

    if (timeSincePlayerSeen.isTimeUp()) //will sort of mitigating stuck enemies looking silly
    {
        state = State::searching;
        this->currentRoamPoint = lastKnownPlayerPos;
        this->searchCounter.reset();
    }

    if (this->lastKnownPlayerPos.x < movement->transform.pos.x)
    {
        moveLeft(movement, sprite);
    }

    else if (this->lastKnownPlayerPos.x >= movement->transform.pos.x)
    {
        moveRight(movement, sprite);
    }


    if (lengthSquared(movement->transform.pos + eyeLevel - lastKnownPlayerPos) < this->attackDistance * this->attackDistance)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            this->attackChargeTimer.reset();
        }
    }

    if (timeSincePlayerSeen > 200 && abs(movement->transform.pos.x - lastKnownPlayerPos.x) < 10)
    {
        state = State::searching;
        this->currentRoamPoint = lastKnownPlayerPos;
        this->searchCounter.reset();
    }
}

void GroundAIComp::updateReturn(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    if (this->currentRoamPoint.x < movement->transform.pos.x)
    {
        moveLeft(movement, sprite);
    }

    else if (this->currentRoamPoint.x >= movement->transform.pos.x)
    {
        moveRight(movement, sprite);
    }

    if (abs(movement->transform.pos.x - currentRoamPoint.x) < 10)
    {
        state = State::idle;
    }
}

void GroundAIComp::updateSearch(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    updateIdle(movement, collider, dt, sprite);

    if (searchCounter.update(dt))
    {
        this->currentRoamPoint = getStartPoint();
        this->state = State::returning;
    }
}

void GroundAIComp::updateStunned(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    movement->acceleration.x = 0;
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

void GroundAIComp::handleCollision(MovementComp* movement, ColliderComp* collider, const ColliderComp* otherCollider)
{
    if (otherCollider->hasComponent(ColliderKeys::ground) || otherCollider->hasComponent(ColliderKeys::platform))
    {
        //walking on ground
        if (movement->momentum.y > 0 && ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
        {
            movement->momentum.y = 0;
            movement->transform.pos.y = otherCollider->up() - collider->height();
            movement->grounded = true;
        }

        //smackin into roof
        if (ColliderComp::intersects(otherCollider->getDownBox(), collider->getUpBox()))
        {
            movement->momentum.y = 0;
            movement->transform.pos.y = otherCollider->down();
        }

        if (ColliderComp::intersects(otherCollider->getLeftBox(), collider->getRightBox()))
        {
            movement->momentum.x *= -0.5f;
            movement->transform.pos.x = otherCollider->left() - collider->width();
            movement->jump();
        }

        if (ColliderComp::intersects(otherCollider->getRightBox(), collider->getLeftBox()))
        {
            movement->momentum.x *= -0.5f;
            movement->transform.pos.x = otherCollider->right();
            movement->jump();
        }
    }
}
