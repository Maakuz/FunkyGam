#include "Boss.h"
#include <fstream>

Boss::Boss(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset):
    Collidable(pos, size)
{
    AnimatedSpriteComp* sprite = new AnimatedSpriteComp(data, pos);
    addComponent<AnimatedSpriteComp>(sprite);
    
    MovementComp* movement = new MovementComp;
    addComponent<MovementComp>(movement);

    addComponent<HealthComp>(new HealthComp);

    m_interrupting = false;
    m_prevHealth = 0;
    movement->transform.pos = pos;

    getColliderComp()->addComponent(ColliderKeys::enemy);

    sprite->spriteOffset.y = -(abs(sprite->getTextureRect().height) - size.y);
    sprite->spriteOffset += offset;
}

void Boss::update(float dt, sf::Vector2f playerPos)
{
    MovementComp* movement = getMovementComp();
    AnimatedSpriteComp* sprite = getAnimatedSpriteComp();

    movement->update(dt);
    getColliderComp()->setPosition(movement->transform.pos);
    sprite->setPosition(movement->transform.pos);
    sprite->updateAnimation(dt);

    if (abs(movement->momentum.x) < movement->walkSpeed * 0.75f && !sprite->isIdle())
        sprite->pauseAnimation();

    else if (abs(movement->momentum.x) > movement->walkSpeed * 0.75f)
        sprite->resumeAnimation();
}

bool Boss::isHealthChanged()
{
    return getComponent<HealthComp>()->getCurrentHealth() != m_prevHealth;
}

void Boss::spawn(sf::Vector2f pos)
{
    getComponent<MovementComp>()->transform.pos = pos;
}

std::istream& operator>>(std::istream& in, Boss& boss)
{
    std::string trash;
    int health = 0;
    in >> trash;

    in >> trash >> health;
    in >> trash >> boss.getComponent<MovementComp>()->mass;
    in >> trash >> boss.getComponent<MovementComp>()->walkSpeed;

    boss.getComponent<HealthComp>()->setMaxHealth(health);
    boss.getComponent<HealthComp>()->fillHealth();
    boss.m_prevHealth = health;

    boss.readSpecific(in);
    return in;
}

void Boss::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*getComponent<AnimatedSpriteComp>(), states);
}