#include "Enemy.h"
#include <string>

Enemy::Enemy(AnimationData data, sf::Vector2f pos)
    :MovingEntity(data, pos)
{
    this->collisionBox.addComponent(CollisionBox::colliderComponents::character);
    this->roamDecisionCounter = Counter(2500 + rand() % 1000);
    this->timeSincePlayerSeen = Counter(10000);
    this->roamDistance = 0;
    this->decisionTime = true;
    this->startPoint = pos;
    this->lastKnownPlayerPos = pos;
    this->state = State::idle;
    this->facingDir = Direction::none;
    this->eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
    this->health = 0;
}

void Enemy::update(float dt)
{
    if (this->roamDecisionCounter.update(dt))
    {
        this->decisionTime = true;
        this->roamDecisionCounter.reset();
    }
    timeSincePlayerSeen.update(dt);
    MovingEntity::update(dt);
}

void Enemy::notifyEnemy(sf::Vector2f playerPos)
{
    this->lastKnownPlayerPos = playerPos;
    this->state = State::chasing;
    this->timeSincePlayerSeen.reset();
}

void Enemy::spawn(sf::Vector2f pos)
{
    this->pos = pos;
    this->startPoint = pos;
    this->lastKnownPlayerPos = pos;
    this->currentRoamPoint = pos;
    this->state = State::idle;
    this->facingDir = Direction::none;
}

bool Enemy::isAlive()
{
    return this->health > 0;
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

std::istream& operator>>(std::istream& in, Enemy& enemy)
{
    std::string trash;

    while (trash != "[EnemyData]")
        in >> trash;


    in >> trash >> enemy.roamDistance;
    in >> trash >> enemy.health;


    enemy.readSpecific(in);
    return in;
}
