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
    addComponent<DamageComp>(new DamageComp);
    this->ai.attackDistance = 64;
    this->ai.eyeLevel.x = data.spriteSheet->getSize().x / data.frameCount.x / 2.f;
    this->ai.eyeLevel.y = data.spriteSheet->getSize().y / data.frameCount.y * 0.2;
    getColliderComp()->addComponent(ColliderKeys::enemy);
    getColliderComp()->addComponent(ColliderKeys::grunt);
}

void Grunt::update(float dt)
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
        if (flying)
            sprite->setAnimation(2);

        else
            sprite->setAnimation(1);
        updateAttack(dt);
        break;

    case AIComp::State::searching:
        sprite->setAnimation(0);

        if (ai.searchCounter < 1)
            drawQuestion.reset();

        ai.updateSearch(movement, collider, dt, sprite);
        
        break;

    case AIComp::State::returning:
        sprite->setAnimation(0);
        ai.updateReturn(movement, collider, dt, sprite);
        break;

    case AIComp::State::stunned:
        sprite->setAnimation(3);
        ai.updateStunned(movement, collider, dt, sprite);
        break;
    }

    Enemy::updateEnemy(dt);
}

void Grunt::updateAttack(float dt)
{
    MovementComp* movement = getMovementComp();
    movement->acceleration.x = 0;
    if (ai.attackChargeTimer.update(dt) && !flying)
    {
        if (ai.facingDir == AIComp::Direction::left)
            movement->momentum = sf::Vector2f(-attackMomentum.x, -attackMomentum.y);

        else
            movement->momentum = sf::Vector2f(attackMomentum.x, -attackMomentum.y);

        movement->grounded = false;
        this->flying = true;
    }

    if (flying && movement->grounded)
    {
        this->flying = false;
        ai.setState(AIComp::State::chasing);
    }
}

std::istream& Grunt::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> getMovementComp()->jumpHeight;
    in >> trash >> attackMomentum.x >> attackMomentum.y;
    in >> trash >> ai.attackDistance;
    in >> trash >> getComponent<DamageComp>()->damage;
    in >> trash >> ai.attackChargeTimer.stopValue;
    in >> trash >> ai.searchCounter.stopValue;
    return in;
}

void Grunt::handleCollision(const Collidable* collidable)
{
    const ColliderComp* otherCollider = collidable->getColliderComp();
    ColliderComp* collider = getColliderComp();
    MovementComp* movement = getMovementComp();

    ai.handleCollision(movement, collider, otherCollider);

    if (flying && !otherCollider->hasComponent(ColliderKeys::player))
    {
        this->flying = false;
        this->ai.setState(AIComp::State::stunned);
    }

    else if (flying && otherCollider->hasComponent(ColliderKeys::player))
    {
        const MovementComp* playerMovement = collidable->getComponent<MovementComp>();

        movement->addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider->getCenterPos(), otherCollider->getCenterPos(), movement->momentum, playerMovement->momentum, movement->mass, playerMovement->mass));
    }

    const DamageComp* damage = collidable->getComponent<DamageComp>();
    if (damage)
    {
        if (damage->origin != DamageComp::DamageOrigin::enemies)
            getComponent<HealthComp>()->takeDamage(damage->damage);
    }
}

void Grunt::handleExplosion(const Explosion& explosion)
{
    MovementComp* movement = getMovementComp();
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(getColliderComp()->getCenterPos());
        this->getComponent<HealthComp>()->takeDamage(damage);
    }

    if (ai.getState() != AIComp::State::stunned && ai.getState() != AIComp::State::chasing && ai.getState() != AIComp::State::attacking)
    {
        this->ai.currentRoamPoint = explosion.center;
        ai.setState(AIComp::State::searching);
        this->drawQuestion.reset();
        ai.searchCounter.reset();

        if (explosion.center.x < movement->transform.pos.x)
            this->ai.moveLeft(movement, getAnimatedSpriteComp());

        else
            ai.moveRight(movement, getAnimatedSpriteComp());
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
