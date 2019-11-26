#include "Grunt.h"
#include "Game/Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"
#include <string>
#include "Game/Item/Projectile/Throwables/Throwable.h"
#include "Game/Item/Projectile//Spell/Fireball.h"
#include "Game/Entities/Player.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset)
    :Enemy(data, pos, size, offset),
    ai(pos, size)
{
    this->damage = 0;
    this->ai.attackDistance = 64;
    this->ai.eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->ai.eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
    this->ai.collider.addComponent(ColliderKeys::character);
    this->ai.collider.addComponent(ColliderKeys::grunt);
}

void Grunt::update(float dt)
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
        if (flying)
            sprite.setAnimation(2);

        else
            sprite.setAnimation(1);
        updateAttack(dt);
        break;

    case AIComp::State::searching:
        sprite.setAnimation(0);

        if (ai.searchCounter < 1)
            drawQuestion.reset();

        ai.updateSearch(dt, &sprite);
        
        break;

    case AIComp::State::returning:
        sprite.setAnimation(0);
        ai.updateReturn(dt, &sprite);
        break;

    case AIComp::State::stunned:
        sprite.setAnimation(3);
        ai.updateStunned(dt, &sprite);
        break;
    }

    Enemy::updateEnemy(dt);
}

void Grunt::updateAttack(float dt)
{
    this->ai.movement.acceleration.x = 0;
    if (ai.attackChargeTimer.update(dt) && !flying)
    {
        if (ai.facingDir == AIComp::Direction::left)
            this->ai.movement.momentum = sf::Vector2f(-attackMomentum.x, -attackMomentum.y);

        else
            this->ai.movement.momentum = sf::Vector2f(attackMomentum.x, -attackMomentum.y);

        this->ai.movement.grounded = false;
        this->flying = true;
    }

    if (flying && ai.movement.grounded)
    {
        this->flying = false;
        ai.setState(AIComp::State::chasing);
    }
}

std::istream& Grunt::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> ai.movement.jumpHeight;
    in >> trash >> attackMomentum.x >> attackMomentum.y;
    in >> trash >> ai.attackDistance;
    in >> trash >> damage;
    in >> trash >> ai.attackChargeTimer.stopValue;
    in >> trash >> ai.searchCounter.stopValue;
    return in;
}

void Grunt::handleCollision(const Collidable* collidable)
{
    ai.handleCollision(collidable);

    if (flying && !collidable->getCollider().hasComponent(ColliderKeys::player))
    {
        this->flying = false;
        this->ai.setState(AIComp::State::stunned);
    }

    else if (flying && collidable->getCollider().hasComponent(ColliderKeys::player))
    {
        const Player* ptr = dynamic_cast<const Player*>(collidable);
        
        ai.movement.addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(ai.collider.getCenterPos(), collidable->getCollider().getCenterPos(), ai.movement.momentum, ptr->getMovementComp().momentum, ai.movement.mass, ptr->getMovementComp().mass));
    }

    if (collidable->getCollider().hasComponent(ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collidable);
        this->health.takeDamage(throwable->getDamage());
    }

    else if (collidable->getCollider().hasComponent(ColliderKeys::fireball))
        this->health.takeDamage(dynamic_cast<const Fireball*>(collidable)->getDamage());
}

void Grunt::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(this->ai.collider.getCenterPos());
        this->health.takeDamage(damage);
    }

    if (ai.getState() != AIComp::State::stunned && ai.getState() != AIComp::State::chasing && ai.getState() != AIComp::State::attacking)
    {
        this->ai.currentRoamPoint = explosion.center;
        ai.setState(AIComp::State::searching);
        this->drawQuestion.reset();
        ai.searchCounter.reset();

        if (explosion.center.x < this->ai.movement.transform.pos.x)
            this->ai.moveLeft(&sprite);

        else
            ai.moveRight(&sprite);
    }

    switch (explosion.type)
    {
    case ExplosionType::flash:
        ai.setState(AIComp::State::stunned);
        ai.stunCounter.reset();
        break;

    default:
        break;
    }
}
