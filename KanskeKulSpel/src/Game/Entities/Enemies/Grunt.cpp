#include "Grunt.h"
#include "Game/Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"
#include <string>
#include "Game/Item/Throwables/Throwable.h"
#include "Game/Entities/Player.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos, UIHandler* ui, sf::Vector2f size, sf::Vector2f offset)
    :Enemy(data, pos, ui, size, offset)
{
    this->flying = false;
    this->forcedDirection = Direction::none;
    this->damage = 0;
    this->attackDistance = 64;
    this->attackChargeTimer = Counter(1000);
    this->collider.addComponent(ColliderKeys::grunt);
}

void Grunt::update(float dt)
{
    switch (state)
    {
    case Enemy::State::idle:
        sprite.setAnimation(0);
        this->movement.walkSpeed = idleSpeed;
        updateIdle(dt);
        break;

    case Enemy::State::chasing:
        sprite.setAnimation(0);
        this->movement.walkSpeed = chaseSpeed;
        updateChasing(dt);
        break;

    case Enemy::State::attacking:
        if (flying)
            sprite.setAnimation(2);

        else
            sprite.setAnimation(1);
        updateAttack(dt);
        break;

    case Enemy::State::searching:
        sprite.setAnimation(0);
        updateIdle(dt);
        if (searchCounter.update(dt))
        {
            this->currentRoamPoint = getStartPoint();
            this->state = State::returning;
        }
        break;

    case Enemy::State::returning:
        sprite.setAnimation(0);
        updateReturning(dt);
        break;

    case Enemy::State::stunned:
        sprite.setAnimation(3);
        this->movement.acceleration.x = 0;
        if (this->stunCounter.update(dt))
        {
            this->stunCounter.reset();

            if (!timeSincePlayerSeen.isTimeUp())
                this->state = State::chasing;

            else
            {
                this->state = State::searching;
                this->currentRoamPoint = getLastKnownPos();
                this->searchCounter.reset();
            }
        }
        break;
    }

    Enemy::updateEnemy(dt);
}

void Grunt::updateIdle(float dt)
{
    if (isDesicionTime() && this->forcedDirection == Direction::none)
    {
        int r = rand() % 3;
        printCon(std::to_string(r));
        switch (r)
        {
        case 0:
            this->movement.acceleration.x = 0;
            break;

        case 1:
            moveLeft();
            break;

        case 2:
            moveRight();
            break;

        default:
            this->movement.acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        if (this->forcedDirection == Direction::right)
            moveRight();

        else
            moveLeft();

        this->forcedDirection = Direction::none;

        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->getPosition() - this->currentRoamPoint;
    if (length(this->getPosition() - this->currentRoamPoint) > this->roamDistance)
    {
        if (roampointToPos.x > 0)
            forcedDirection = Direction::left;

        else
            forcedDirection = Direction::right;
    }


}

void Grunt::updateChasing(float dt)
{
    if (timeSincePlayerSeen.isTimeUp()) //will sort of mitigating stuck enemies looking silly
    {
        drawQuestion.reset();
        state = State::searching;
        this->currentRoamPoint = getLastKnownPos();
        this->searchCounter.reset();
    }

    if (this->getLastKnownPos().x < this->getPosition().x)
    {
        moveLeft();
    }

    else if (this->getLastKnownPos().x >= this->getPosition().x)
    {
        moveRight();
    }


    if (lengthSquared(getPosition() + eyeLevel - getLastKnownPos()) < this->attackDistance * this->attackDistance)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            this->attackChargeTimer.reset();
        }
    }

    if(timeSincePlayerSeen > 200 && abs(getPosition().x - getLastKnownPos().x) < 10)
    {
        drawQuestion.reset();
        state = State::searching;
        this->currentRoamPoint = getLastKnownPos();
        this->searchCounter.reset();
    }
}

void Grunt::updateReturning(float dt)
{
    if (this->currentRoamPoint.x < this->getPosition().x)
    {
        moveLeft();
    }

    else if (this->currentRoamPoint.x >= this->getPosition().x)
    {
        moveRight();
    }

    if (abs(getPosition().x - currentRoamPoint.x) < 10)
    {
        state = State::idle;
    }
}

void Grunt::updateAttack(float dt)
{
    this->movement.acceleration.x = 0;
    if (attackChargeTimer.update(dt) && !flying)
    {
        if (facingDir == Direction::left)
            this->movement.momentum = sf::Vector2f(-attackMomentum.x, -attackMomentum.y);

        else
            this->movement.momentum = sf::Vector2f(attackMomentum.x, -attackMomentum.y);

        this->movement.grounded = false;
        this->flying = true;
    }
}

std::istream& Grunt::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> movement.jumpHeight;
    in >> trash >> attackMomentum.x >> attackMomentum.y;
    in >> trash >> attackDistance;
    in >> trash >> damage;
    in >> trash >> attackChargeTimer.stopValue;
    in >> trash >> searchCounter.stopValue;
    return in;
}

void Grunt::handleCollision(const Collidable* collidable)
{
    if (collidable->getCollider().hasComponent(ColliderKeys::ground) || collidable->getCollider().hasComponent(ColliderKeys::platform))
    {
        //walking on ground
        if (this->movement.momentum.y > 0 && collidable->getCollider().intersects(collidable->getCollider().getUpBox(), this->collider.getDownBox()))
        {
            this->movement.momentum.y = 0;
            this->pos.y = collidable->getCollider().up() - this->collider.height();
            movement.grounded = true;

            if (flying)
            {
                state = State::chasing;
                flying = false;
            }
        }

        //smackin into roof
        if (collidable->getCollider().intersects(collidable->getCollider().getDownBox(), this->collider.getUpBox()))
        {
            this->movement.momentum.y = 0;
            this->pos.y = collidable->getCollider().down();
        }

        if (collidable->getCollider().intersects(collidable->getCollider().getLeftBox(), this->collider.getRightBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->pos.x = collidable->getCollider().left() - this->collider.width();
            this->movement.jump();
        }

        if (collidable->getCollider().intersects(collidable->getCollider().getRightBox(), this->collider.getLeftBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->pos.x = collidable->getCollider().right();
            this->movement.jump();
        }
    }

    else if (flying && !collidable->getCollider().hasComponent(ColliderKeys::player))
    {
        this->flying = false;
        this->state = State::stunned;
    }

    else if (flying && collidable->getCollider().hasComponent(ColliderKeys::player))
    {
        const Player* ptr = dynamic_cast<const Player*>(collidable);
        
        movement.addCollisionMomentum(ColliderComp::calculateCollisionForceOnObject(collider.getCenterPos(), collidable->getCollider().getCenterPos(), movement.momentum, ptr->getMovementComp().momentum, movement.mass, ptr->getMovementComp().mass));
    }

    if (collidable->getCollider().hasComponent(ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collidable);
        this->health.takeDamage(throwable->getDamage());
    }
}

void Grunt::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(this->collider.getCenterPos());
        this->health.takeDamage(damage);
    }

    if (state != State::stunned && state != State::chasing && state != State::attacking)
    {
        this->currentRoamPoint = explosion.center;
        this->state = State::searching;
        this->drawQuestion.reset();
        searchCounter.reset();

        if (explosion.center.x < this->getPosition().x)
            this->moveLeft();

        else
            moveRight();
    }

    switch (explosion.type)
    {
    case ExplosionType::flash:
        state = State::stunned;
        stunCounter.reset();
        break;

    default:
        break;
    }
}
