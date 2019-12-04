#include "Bird.h"
#include "Game/Item/Projectile/ProjectileHandler.h"
#include "Misc/ConsoleWindow.h"

Bird::Bird(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset)
    :Enemy(data, pos, size, offset),
    ai(pos, size)
{
    getMovementComp()->setIgnoreGravity(true);
    this->inBombingRange = false;
    startBombing = false;
    this->ai.eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->ai.eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
    getColliderComp()->addComponent(ColliderKeys::enemy);
}

Bird::~Bird()
{
}

void Bird::update(float dt)
{
    AnimatedSpriteComp* sprite = getAnimatedSpriteComp();
    ColliderComp* collider = getColliderComp();
    MovementComp* movement = getMovementComp();

    switch (ai.getState())
    {
    case AIComp::State::idle:
        sprite->setAnimation(0);
        ai.updateIdle(movement, collider, dt, sprite);
        break;

    case AIComp::State::chasing:
        sprite->setAnimation(0);
        ai.updateChasing(movement, collider, dt, sprite);
        break;

    case AIComp::State::attacking:
        sprite->setAnimation(1);
        updateAttack(dt);
        break;

    case AIComp::State::returning:
        sprite->setAnimation(0);
        ai.updateReturn(movement, collider, dt, sprite);
        break;

    case AIComp::State::stunned:
        ai.updateStunned(movement, collider, dt, sprite);
        break;
    }

    Enemy::updateEnemy(dt);
}

void Bird::updateAttack(float dt)
{
    ColliderComp* collider = getColliderComp();
    if (lengthSquared(ai.attackDestination - collider->getCenterPos()) < 150 * 150)
        this->startBombing = true;

    if (startBombing && bombCounter.update(dt))
    {
        ProjectileHandler::addThrowable(11, collider->getCenterPos(), getMovementComp()->momentum, DamageComp::DamageOrigin::enemies);
        bombCounter.reset();

        if (bombsPerAttack.update(1))
        {
            ai.setState(AIComp::State::chasing);
            startBombing = false;
            bombsPerAttack.reset();
        }
    }
}

void Bird::handleCollision(const Collidable* collidable)
{
    const ColliderComp* otherCollider = collidable->getComponent<ColliderComp>();
    ai.handleCollision(getMovementComp(), getColliderComp(), otherCollider);

    if (otherCollider->hasComponent(ColliderKeys::throwable))
    {
        if (collidable->getComponent<LogisticsComp>()->id != 11) //is immune to birdbombs
            this->getComponent<HealthComp>()->takeDamage(collidable->getComponent<DamageComp>()->damage);
    }

    else if (otherCollider->hasComponent(ColliderKeys::fireball))
        this->getComponent<HealthComp>()->takeDamage(collidable->getDamageComp()->damage);
}

void Bird::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0 && explosion.type != ExplosionType::bird)
    {
        int damage = explosion.calculateDamage(getColliderComp()->getCenterPos());
        this->getComponent<HealthComp>()->takeDamage(damage);
    }
}


std::istream& Bird::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> ai.attackCounter.stopValue;
    in >> trash >> bombsPerAttack.stopValue;
    in >> trash >> bombCounter.stopValue;

    return in;
}
