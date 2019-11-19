#include "AIComp.h"

AIComp::AIComp(sf::Vector2f pos, sf::Vector2f size):
    collider(size, pos)
{
    this->timeSincePlayerSeen = Counter(10000);

    this->roamDistance = 0;
    this->chaseSpeed = 0;
    this->idleSpeed = 0;
    this->sightMultiplier = 0;
    this->sightRadius = 0;
    this->decisionTime = true;
    this->state = State::idle;
    this->facingDir = Direction::none;
    this->forcedDirection = Direction::none;



}

void AIComp::baseUpdate(float dt)
{
    if (this->roamDecisionCounter.update(dt))
    {
        this->decisionTime = true;
        this->roamDecisionCounter.reset();
    }
    timeSincePlayerSeen.update(dt);




    this->movement.update(dt);
    collider.setPosition(movement.transform.pos);
}

void AIComp::notify(sf::Vector2f pos)
{
    this->lastKnownPlayerPos = pos;
    this->state = State::chasing;
    this->timeSincePlayerSeen.reset();
}

void AIComp::setState(State state)
{
    this->state = state;
}

void AIComp::moveLeft(SpriteComp* sprite)
{
    this->movement.acceleration.x = -1;
    this->facingDir = Direction::left;
    if (!sprite)
        return;

    if (!sprite->isFlippedHorizontally())
        sprite->flipHorizontally();
}

void AIComp::moveRight(SpriteComp* sprite)
{
    this->movement.acceleration.x = 1;
    this->facingDir = Direction::right;
    if (!sprite)
        return;

    if (sprite->isFlippedHorizontally())
        sprite->flipHorizontally();
}

void AIComp::faceLeft(SpriteComp* sprite)
{
    this->facingDir = Direction::left;
    if (!sprite)
        return;
    if (!sprite->isFlippedHorizontally())
        sprite->flipHorizontally();
}

void AIComp::faceRight(SpriteComp* sprite)
{
    this->facingDir = Direction::right;
    if (!sprite)
        return;
    if (sprite->isFlippedHorizontally())
        sprite->flipHorizontally();
}

void AIComp::desicionTimeOver()
{
    this->decisionTime = false;
    this->roamDecisionCounter.reset();
}