#include "Bird.h"
#include "Game/Item/ProjectileHandler.h"
#include "Misc/ConsoleWindow.h"

Bird::Bird(AnimationData data, sf::Vector2f pos, UIHandler* ui, sf::Vector2f size, sf::Vector2f offset)
    :Enemy(data, pos, ui, size, offset)
{
    movement.setIgnoreGravity(true);
    this->inBombingRange = false;
    forcedDirResolved = false;
    startBombing = false;
}

Bird::~Bird()
{
}

void Bird::update(float dt)
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
        sprite.setAnimation(1);
        updateAttack(dt);
        break;

    case Enemy::State::returning:
        sprite.setAnimation(0);
        updateReturning(dt);
        break;

    case Enemy::State::stunned:
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
            }
        }
        break;
    }

    Enemy::updateEnemy(dt);
}


void Bird::updateIdle(float dt)
{
    if (isDesicionTime() && this->forcedDirection == Direction::none)
    {
        int x = rand() % 2;
        int y = (rand() % 100) -50;
        switch (x)
        {
        case 0:
            moveLeft();
            break;

        case 1:
            moveRight();
            break;
        default:
            this->movement.acceleration.x = 0;
            this->movement.acceleration.y = 0;
            break;
        }
        this->movement.acceleration.y = y / 50.f;
        normalize(this->movement.acceleration);
        forcedDirResolved = false;
        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        this->movement.acceleration = this->currentRoamPoint - this->collider.getCenterPos();
        normalize(this->movement.acceleration);
        forcedDirection = Direction::none;

        if (movement.acceleration.x > 0)
            faceRight();

        else
            faceLeft();

        forcedDirResolved = true;
        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->getPosition() - this->currentRoamPoint;
    if (length(this->pos - this->currentRoamPoint) > this->roamDistance && !forcedDirResolved)
    {
        forcedDirection = Direction::right;
        forcedDirResolved = false;
    }
}

void Bird::updateChasing(float dt)
{
    if (getLastKnownPos().x < collider.getCenterPos().x)
        faceLeft();

    else
        faceRight();

    int x = (rand() % 400) - 200;
    if (isDesicionTime())
    {
        x = (rand() % 400) - 200;

        desicionTimeOver();
    }


    this->movement.acceleration = getLastKnownPos() + sf::Vector2f(x, -200) - collider.getCenterPos();

    if (lengthSquared(movement.acceleration) > 1.f)
        normalize(movement.acceleration);

    if (attackCounter.update(dt) && lengthSquared(getLastKnownPos() - collider.getCenterPos()) < 500 * 500)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            movement.momentum.x = 0;
            movement.momentum.y = 0;
            attackCounter.reset();
            attackDestination = getLastKnownPos() + sf::Vector2f(0, -190);
            this->movement.acceleration = attackDestination - collider.getCenterPos();
            normalize(movement.acceleration);
            movement.acceleration *= 1.3f;
        }

        else
            state = State::returning;
    }

}

void Bird::updateReturning(float dt)
{
    this->movement.acceleration = this->currentRoamPoint - this->collider.getCenterPos();
    normalize(this->movement.acceleration);

    if (movement.acceleration.x > 0)
        faceRight();

    else
        faceLeft();

    if (lengthSquared(this->currentRoamPoint - collider.getCenterPos()) < 250)
        state = State::idle;
}

void Bird::updateAttack(float dt)
{
    if (lengthSquared(attackDestination - collider.getCenterPos()) < 150 * 150)
        this->startBombing = true;

    if (startBombing && bombCounter.update(dt))
    {
        ProjectileHandler::addThrowable(11, collider.getCenterPos(), this->movement.momentum, this);
        bombCounter.reset();

        if (bombsPerAttack.update(1))
        {
            state = State::chasing;
            startBombing = false;
            bombsPerAttack.reset();
        }
    }
}

void Bird::handleCollision(const Collidable* collidable)
{
    if (collidable->getCollider().hasComponent(ColliderKeys::ground))
    {

        if (collidable->getCollider().intersects(collidable->getCollider().getLeftBox(), this->collider.getRightBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.acceleration.x *= -1;
            this->facingDir = Direction::left;
            this->sprite.flipHorizontally();
            this->pos.x = collidable->getCollider().left() - this->collider.width();
            this->movement.jump();
        }

        else if (collidable->getCollider().intersects(collidable->getCollider().getRightBox(), this->collider.getLeftBox()))
        {
            this->movement.momentum.x *= -0.5f;
            this->movement.acceleration.x *= -1;
            this->facingDir = Direction::right;
            this->sprite.flipHorizontally();
            this->pos.x = collidable->getCollider().right();
            this->movement.jump();
        }


        //walking on ground
        else if (this->movement.momentum.y > 0 && collidable->getCollider().intersects(collidable->getCollider().getUpBox(), this->collider.getDownBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.acceleration.y *= -1;
            this->pos.y = collidable->getCollider().up() - this->collider.height();
            movement.grounded = true;
        }

        //smackin into roof
        else if (collidable->getCollider().intersects(collidable->getCollider().getDownBox(), this->collider.getUpBox()))
        {
            this->movement.momentum.y = 0;
            this->movement.acceleration.y *= -1;
            this->pos.y = collidable->getCollider().down();
        }
    }

    else if (collidable->getCollider().hasComponent(ColliderKeys::throwable))
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
        int damage = explosion.calculateDamage(this->collider.getCenterPos());
        this->health.takeDamage(damage);
    }
}


std::istream& Bird::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> attackCounter.stopValue;
    in >> trash >> bombsPerAttack.stopValue;
    in >> trash >> bombCounter.stopValue;

    return in;
}
