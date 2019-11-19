#include "Enemy.h"
#include <string>
#include "Game/Handlers/TextureHandler.h"

Enemy::Enemy(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    Entity(pos),
    sprite(data, pos),
    collider(size, pos)
{
    this->prevHealth = 0;
    this->collider.addComponent(ColliderKeys::character);
    this->roamDecisionCounter = Counter(2500 + rand() % 1000);
    this->timeSincePlayerSeen = Counter(10000);
    this->drawExclamation = Counter(1000);
    this->drawQuestion = Counter(1000);
    this->drawExclamation.counter = 1000;
    this->drawQuestion.counter = 1000;
    this->roamDistance = 0;
    this->chaseSpeed = 0;
    this->idleSpeed = 0;
    this->roamDistance = 0;
    this->sightMultiplier = 0;
    this->sightRadius = 0;
    this->decisionTime = true;
    this->startPoint = pos;
    this->lastKnownPlayerPos = pos;
    this->state = State::idle;
    this->facingDir = Direction::none;
    this->forcedDirection = Direction::none;
    this->eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
    this->question.setTexture(*TextureHandler::get().getTexture(20));
    this->exclamation.setTexture(*TextureHandler::get().getTexture(19));

    this->sprite.spriteOffset.y = -(abs(sprite.getTextureRect().height) - size.y);
    this->sprite.spriteOffset += offset;
}

void Enemy::updateEnemy(float dt)
{
    if (this->roamDecisionCounter.update(dt))
    {
        this->decisionTime = true;
        this->roamDecisionCounter.reset();
    }
    timeSincePlayerSeen.update(dt);

    drawExclamation.update(dt);
    drawQuestion.update(dt);

    sf::Vector2f iconPos(getPosition() + sprite.spriteOffset + sf::Vector2f(this->collider.getSize().x / 2 + 10, 0));
    this->exclamation.setPosition(iconPos);
    this->question.setPosition(iconPos);

    this->prevHealth = this->health.getHealth();

    if (abs(movement.momentum.x) < this->movement.walkSpeed * 0.75f && !sprite.isIdle())
        sprite.pauseAnimation();

    else if (abs(movement.momentum.x) > this->movement.walkSpeed * 0.75f)
        sprite.resumeAnimation();

    this->pos = movement.update(dt, this->pos);
    sprite.updateAnimation(dt);
    sprite.update(this->pos);
    collider.setPosition(this->pos);
}

void Enemy::notifyEnemy(sf::Vector2f playerPos)
{
    if (state != State::chasing)
        this->drawExclamation.reset();

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
    return this->health.isAlive();
}

bool Enemy::isHealthChanged()
{
    return health.getHealth() != prevHealth;
}

float Enemy::getHealthPercentage() const
{
    return float(health.getHealth()) / health.getMaxHealth();
}

float Enemy::getVisionRatingAt(float distance) const
{
    //I plotted this graph, it looks nice
    float x = distance;
    float a = 100;
    float b = sightMultiplier;
    float h = sightRadius;
    return (a * b * std::pow(x - h, 2)) / std::pow(h, 2);
}

sf::Vector2f Enemy::getEyePos() const
{
    return this->getPosition() + this->eyeLevel;
}

void Enemy::moveLeft()
{
    this->movement.acceleration.x = -1;
    this->facingDir = Direction::left;
    if (!this->sprite.isFlippedHorizontally())
        this->sprite.flipHorizontally();
}

void Enemy::moveRight()
{
    this->movement.acceleration.x = 1;
    if (this->sprite.isFlippedHorizontally())
        this->sprite.flipHorizontally();
    this->facingDir = Direction::right;
}

void Enemy::faceLeft()
{
    this->facingDir = Direction::left;
    if (!this->sprite.isFlippedHorizontally())
        this->sprite.flipHorizontally();
}

void Enemy::faceRight()
{
    if (this->sprite.isFlippedHorizontally())
        this->sprite.flipHorizontally();
    this->facingDir = Direction::right;
}

void Enemy::desicionTimeOver()
{ 
    this->decisionTime = false;
    this->roamDecisionCounter.reset();
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!drawExclamation.isTimeUp())
        target.draw(exclamation, states);

    else if (!drawQuestion.isTimeUp())
        target.draw(question, states);

    target.draw(sprite, states);
}

std::istream& operator>>(std::istream& in, Enemy& enemy)
{
    std::string trash;
    int health = 0;
    in >> trash;


    in >> trash >> enemy.roamDistance;
    in >> trash >> health;
    in >> trash >> enemy.movement.mass;
    in >> trash >> enemy.sightRadius;
    in >> trash >> enemy.sightMultiplier;
    in >> trash >> enemy.chaseSpeed;
    in >> trash >> enemy.idleSpeed;
    in >> trash >> enemy.stunCounter.stopValue;
    in >> trash >> enemy.roamDecisionCounter.stopValue;

    enemy.roamDecisionCounter.stopValue += rand() % 1000;
    enemy.health.setMaxHealth(health);
    enemy.health.fillHealth();
    enemy.prevHealth = health;

    enemy.readSpecific(in);
    return in;
}
