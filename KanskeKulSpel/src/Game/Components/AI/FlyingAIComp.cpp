#include "FlyingAIComp.h"

FlyingAIComp::FlyingAIComp(sf::Vector2f pos, sf::Vector2f size):
    AIComp(pos, size)
{
    this->forcedDirResolved = false;
}

void FlyingAIComp::updateIdle(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    movement->walkSpeed = idleSpeed;

    if (isDesicionTime() && this->forcedDirection == Direction::none)
    {
        int x = rand() % 2;
        int y = (rand() % 100) - 50;
        switch (x)
        {
        case 0:
            moveLeft(movement, sprite);
            break;

        case 1:
            moveRight(movement, sprite);
            break;
        default:
            movement->acceleration.x = 0;
            movement->acceleration.y = 0;
            break;
        }
        movement->acceleration.y = y / 50.f;
        normalize(movement->acceleration);
        forcedDirResolved = false;
        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        movement->acceleration = this->currentRoamPoint - collider->getCenterPos();
        normalize(movement->acceleration);
        forcedDirection = Direction::none;

        if (movement->acceleration.x > 0)
            faceRight(sprite);

        else
            faceLeft(sprite);

        forcedDirResolved = true;
        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = movement->transform.pos - this->currentRoamPoint;
    if (length(movement->transform.pos - this->currentRoamPoint) > this->roamDistance && !forcedDirResolved)
    {
        forcedDirection = Direction::right;
        forcedDirResolved = false;
    }
}

void FlyingAIComp::updateChasing(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    movement->walkSpeed = chaseSpeed;

    if (lastKnownPlayerPos.x < collider->getCenterPos().x)
        faceLeft(sprite);

    else
        faceRight(sprite);

    int x = (rand() % 400) - 200;
    if (isDesicionTime())
    {
        x = (rand() % 400) - 200;

        desicionTimeOver();
    }


    movement->acceleration = lastKnownPlayerPos + sf::Vector2f(x, -200) - collider->getCenterPos();

    if (lengthSquared(movement->acceleration) > 1.f)
        normalize(movement->acceleration);

    if (attackCounter.update(dt) && lengthSquared(lastKnownPlayerPos - collider->getCenterPos()) < 500 * 500)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            movement->momentum.x = 0;
            movement->momentum.y = 0;
            attackCounter.reset();
            attackDestination = lastKnownPlayerPos + sf::Vector2f(0, -190);
            movement->acceleration = attackDestination - collider->getCenterPos();
            normalize(movement->acceleration);
            movement->acceleration *= 1.3f;
        }

        else
            state = State::returning;
    }

}

void FlyingAIComp::updateReturn(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
{
    movement->acceleration = this->currentRoamPoint - collider->getCenterPos();
    normalize(movement->acceleration);

    if (movement->acceleration.x > 0)
        faceRight(sprite);

    else
        faceLeft(sprite);

    if (lengthSquared(this->currentRoamPoint - collider->getCenterPos()) < 250)
        state = State::idle;
}

void FlyingAIComp::updateStunned(MovementComp* movement, ColliderComp* collider, float dt, SpriteComp* sprite)
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
        }
    }
}

void FlyingAIComp::handleCollision(MovementComp* movement, ColliderComp* collider, const ColliderComp* otherCollider, SpriteComp* sprite)
{
    if (otherCollider->hasComponent(ColliderKeys::ground))
    {

        if (ColliderComp::intersects(otherCollider->getLeftBox(), collider->getRightBox()))
        {
            movement->momentum.x *= -0.5f;
            movement->acceleration.x *= -1;
            this->facingDir = Direction::left;
            movement->transform.pos.x = otherCollider->left() - collider->width();
            movement->jump();

            if (sprite)
                sprite->flipHorizontally();
        }

        else if (ColliderComp::intersects(otherCollider->getRightBox(), collider->getLeftBox()))
        {
            movement->momentum.x *= -0.5f;
            movement->acceleration.x *= -1;
            this->facingDir = Direction::right;
            movement->transform.pos.x = otherCollider->right();
            movement->jump();

            if (sprite)
                sprite->flipHorizontally();
        }


        //walking on ground
        else if (movement->momentum.y > 0 && ColliderComp::intersects(otherCollider->getUpBox(), collider->getDownBox()))
        {
            movement->momentum.y = 0;
            movement->acceleration.y *= -1;
            movement->transform.pos.y = otherCollider->up() - collider->height();
            movement->grounded = true;
        }

        //smackin into roof
        else if (ColliderComp::intersects(otherCollider->getDownBox(), collider->getUpBox()))
        {
            movement->momentum.y = 0;
            movement->acceleration.y *= -1;
            movement->transform.pos.y = otherCollider->down();
        }
    }

}
