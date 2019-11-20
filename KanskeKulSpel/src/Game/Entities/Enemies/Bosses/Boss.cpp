#include "Boss.h"
#include <fstream>

Boss::Boss(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset):
    sprite(data, pos),
    collider(size, pos)
{
    this->interrupting = false;
    this->prevHealth = 0;
    movement.transform.pos = pos;

    this->sprite.spriteOffset.y = -(abs(sprite.getTextureRect().height) - size.y);
    this->sprite.spriteOffset += offset;
}

void Boss::update(float dt, sf::Vector2f playerPos)
{
    movement.update(dt);
    collider.setPosition(movement.transform.pos);
    sprite.setPosition(movement.transform.pos);
}

bool Boss::isHealthChanged()
{
    return health.getHealth() != prevHealth;
}

void Boss::spawn(sf::Vector2f pos)
{
    this->movement.transform.pos = pos;
}

std::istream& operator>>(std::istream& in, Boss& boss)
{
    std::string trash;
    int health = 0;
    in >> trash;

    in >> trash >> health;
    in >> trash >> boss.movement.mass;
    in >> trash >> boss.movement.walkSpeed;

    boss.health.setMaxHealth(health);
    boss.health.fillHealth();
    boss.prevHealth = health;

    return in;
}

void Boss::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite, states);
}