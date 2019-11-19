#include "Enemy.h"
#include <string>
#include "Game/Handlers/TextureHandler.h"

Enemy::Enemy(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset) :
    sprite(data, pos)
{
    this->prevHealth = 0;
    this->sprite.spriteOffset.y = -(abs(sprite.getTextureRect().height) - size.y);
    this->sprite.spriteOffset += offset;

    this->drawExclamation = Counter(1000);
    this->drawQuestion = Counter(1000);
    this->drawExclamation.counter = 1000;
    this->drawQuestion.counter = 1000;
    this->question.setTexture(*TextureHandler::get().getTexture(20));
    this->exclamation.setTexture(*TextureHandler::get().getTexture(19));
}

void Enemy::updateEnemy(float dt)
{
    getAI()->baseUpdate(dt);

    drawExclamation.update(dt);
    drawQuestion.update(dt);
    sf::Vector2f iconPos(getAI()->movement.transform.pos + sprite.spriteOffset + sf::Vector2f(this->getAI()->collider.getSize().x / 2 + 10, 0));
    this->exclamation.setPosition(iconPos);
    this->question.setPosition(iconPos);

    this->prevHealth = this->health.getHealth();

    if (abs(getAI()->movement.momentum.x) < this->getAI()->movement.walkSpeed * 0.75f && !sprite.isIdle())
        sprite.pauseAnimation();

    else if (abs(getAI()->movement.momentum.x) > this->getAI()->movement.walkSpeed * 0.75f)
        sprite.resumeAnimation();

    sprite.updateAnimation(dt);
    sprite.setPosition(getAI()->movement.transform.pos);
}

void Enemy::notifyEnemy(sf::Vector2f playerPos)
{
    if (getAI()->getState() != AIComp::State::chasing)
        this->drawExclamation.reset();

    getAI()->notify(playerPos);
}

void Enemy::spawn(sf::Vector2f pos)
{
    this->getAI()->movement.transform.pos = pos;
    this->getAI()->setStartPoint(pos);
    this->getAI()->currentRoamPoint = pos;
    this->getAI()->setState(AIComp::State::idle);
    this->getAI()->facingDir = AIComp::Direction::none;
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
    float b = getAI()->sightMultiplier;
    float h = getAI()->sightRadius;
    return (a * b * std::pow(x - h, 2)) / std::pow(h, 2);
}

sf::Vector2f Enemy::getEyePos() const
{
    return this->getAI()->movement.transform.pos + this->getAI()->eyeLevel;
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);

    if (!drawExclamation.isTimeUp())
        target.draw(exclamation, states);

    else if (!drawQuestion.isTimeUp())
        target.draw(question, states);
}

std::istream& operator>>(std::istream& in, Enemy& enemy)
{
    std::string trash;
    int health = 0;
    in >> trash;


    in >> trash >> enemy.getAI()->roamDistance;
    in >> trash >> health;
    in >> trash >> enemy.getAI()->movement.mass;
    in >> trash >> enemy.getAI()->sightRadius;
    in >> trash >> enemy.getAI()->sightMultiplier;
    in >> trash >> enemy.getAI()->chaseSpeed;
    in >> trash >> enemy.getAI()->idleSpeed;
    in >> trash >> enemy.getAI()->stunCounter.stopValue;
    in >> trash >> enemy.getAI()->roamDecisionCounter.stopValue;

    enemy.getAI()->roamDecisionCounter.stopValue += rand() % 1000;
    enemy.health.setMaxHealth(health);
    enemy.health.fillHealth();
    enemy.prevHealth = health;

    enemy.readSpecific(in);
    return in;
}
