#include "Enemy.h"

Enemy::Enemy(AnimationData data, sf::Vector2f pos)
    :MovingEntity(data, pos)
{
    this->roamDistance = 0;
    this->decisionTime = true;
    this->origin = pos;
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
