#include "Enemy.h"

Enemy::Enemy(AnimationData data, sf::Vector2f pos)
    :MovingEntity(data, pos)
{
    this->roamDecisionCounter = Counter(3000);
    this->roamDistance = 0;
    this->decisionTime = true;
    this->startPoint = pos;
    this->lastKnownPlayerPos = pos;
    this->state = State::idle;
    this->facingDir = Direction::none;
    this->eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
}

void Enemy::update(float dt)
{
    if (this->roamDecisionCounter.update(dt))
    {
        this->decisionTime = true;
        this->roamDecisionCounter.reset();
    }

    MovingEntity::update(dt);
}

void Enemy::notifyEnemy(sf::Vector2f playerPos)
{
    this->lastKnownPlayerPos = playerPos;
    this->state = State::chasing;
}

sf::Vector2f Enemy::getEyePos() const
{
    return this->pos + this->eyeLevel;
}

void Enemy::desicionTimeOver() 
{ 
    this->decisionTime = false;
    this->roamDecisionCounter.reset();
}
