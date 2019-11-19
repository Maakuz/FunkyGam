#include "Bird.h"
#include "Game/Item/ProjectileHandler.h"
#include "Misc/ConsoleWindow.h"

Bird::Bird(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset)
    :Enemy(data, pos, size, offset),
    ai(pos, size)
{
    ai.movement.setIgnoreGravity(true);
    this->inBombingRange = false;
    startBombing = false;
    this->ai.eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->ai.eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
    this->ai.collider.addComponent(ColliderKeys::character);
}

Bird::~Bird()
{
}

void Bird::update(float dt)
{
    switch (ai.getState())
    {
    case AIComp::State::idle:
        sprite.setAnimation(0);
        ai.updateIdle(dt, &sprite);
        break;

    case AIComp::State::chasing:
        sprite.setAnimation(0);
        ai.updateChasing(dt, &sprite);
        break;

    case AIComp::State::attacking:
        sprite.setAnimation(1);
        updateAttack(dt);
        break;

    case AIComp::State::returning:
        sprite.setAnimation(0);
        ai.updateReturn(dt, &sprite);
        break;

    case AIComp::State::stunned:
        ai.updateStunned(dt, &sprite);
        break;
    }

    Enemy::updateEnemy(dt);
}

void Bird::updateAttack(float dt)
{
    if (lengthSquared(ai.attackDestination - ai.collider.getCenterPos()) < 150 * 150)
        this->startBombing = true;

    if (startBombing && bombCounter.update(dt))
    {
        ProjectileHandler::addThrowable(11, ai.collider.getCenterPos(), this->ai.movement.momentum, this);
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
    ai.handleCollision(collidable);

    if (collidable->getCollider().hasComponent(ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collidable);
        if (throwable->getID() != 11) //is immune to birdbombs
            this->health.takeDamage(throwable->getDamage());
    }
}

void Bird::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0 && explosion.type != ExplosionType::bird)
    {
        int damage = explosion.calculateDamage(this->ai.collider.getCenterPos());
        this->health.takeDamage(damage);
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
