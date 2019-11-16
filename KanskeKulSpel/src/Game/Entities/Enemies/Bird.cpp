#include "Bird.h"
#include "Game/Item/ProjectileHandler.h"
#include "Misc/ConsoleWindow.h"

Bird::Bird(AnimationData data, sf::Vector2f pos, UIHandler* ui)
    :Enemy(data, pos, ui)
{
    setIgnoreGravity(true);
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
        setAnimation(0);
        this->walkSpeed = idleSpeed;
        updateIdle(dt);
        break;

    case Enemy::State::chasing:
        setAnimation(0);
        this->walkSpeed = chaseSpeed;
        updateChasing(dt);
        break;

    case Enemy::State::attacking:
        setAnimation(1);
        updateAttack(dt);
        break;

    case Enemy::State::returning:
        setAnimation(0);
        updateReturning(dt);
        break;

    case Enemy::State::stunned:
        this->acceleration.x = 0;
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
            this->acceleration.x = 0;
            this->acceleration.y = 0;
            break;
        }
        this->acceleration.y = y / 50.f;
        normalize(this->acceleration);
        forcedDirResolved = false;
        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        this->acceleration = this->currentRoamPoint - this->getCenterPos();
        normalize(this->acceleration);
        forcedDirection = Direction::none;

        if (acceleration.x > 0)
            faceRight();

        else
            faceLeft();

        forcedDirResolved = true;
        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->getPosition() - this->currentRoamPoint;
    if (length(this->getPosition() - this->currentRoamPoint) > this->roamDistance && !forcedDirResolved)
    {
        forcedDirection = Direction::right;
        forcedDirResolved = false;
    }
}

void Bird::updateChasing(float dt)
{
    if (getLastKnownPos().x < getCenterPos().x)
        faceLeft();

    else
        faceRight();

    int x = (rand() % 400) - 200;
    if (isDesicionTime())
    {
        x = (rand() % 400) - 200;

        desicionTimeOver();
    }


    this->acceleration = getLastKnownPos() + sf::Vector2f(x, -200) - getCenterPos();

    if (lengthSquared(acceleration) > 1.f)
        normalize(acceleration);

    if (attackCounter.update(dt) && lengthSquared(getLastKnownPos() - getCenterPos()) < 500 * 500)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            momentum.x = 0;
            momentum.y = 0;
            attackCounter.reset();
            attackDestination = getLastKnownPos() + sf::Vector2f(0, -190);
            this->acceleration = attackDestination - getCenterPos();
            normalize(acceleration);
            acceleration *= 1.3f;
        }

        else
            state = State::returning;
    }

}

void Bird::updateReturning(float dt)
{
    this->acceleration = this->currentRoamPoint - this->getCenterPos();
    normalize(this->acceleration);

    if (acceleration.x > 0)
        faceRight();

    else
        faceLeft();

    if (lengthSquared(this->currentRoamPoint - getCenterPos()) < 250)
        state = State::idle;
}

void Bird::updateAttack(float dt)
{
    if (lengthSquared(attackDestination - getCenterPos()) < 150 * 150)
        this->startBombing = true;

    if (startBombing && bombCounter.update(dt))
    {
        ProjectileHandler::addThrowable(11, getCenterPos(), this->momentum, this);
        bombCounter.reset();

        if (bombsPerAttack.update(1))
        {
            state = State::chasing;
            startBombing = false;
            bombsPerAttack.reset();
        }
    }
}

void Bird::handleCollision(const Entity* collider)
{
    if (collider->getCollider().hasComponent(Collider::ColliderKeys::ground))
    {

        if (collider->getCollider().intersects(collider->getCollider().getLeft(), this->collider.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->acceleration.x *= -1;
            this->facingDir = Direction::left;
            this->flipHorizontally();
            this->setX(collider->left() - this->width());
            this->jump();
        }

        else if (collider->getCollider().intersects(collider->getCollider().getRight(), this->collider.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->acceleration.x *= -1;
            this->facingDir = Direction::right;
            this->flipHorizontally();
            this->setX(collider->right());
            this->jump();
        }


        //walking on ground
        else if (this->momentum.y > 0 && collider->getCollider().intersects(collider->getCollider().getUp(), this->collider.getDown()))
        {
            this->momentum.y = 0;
            this->acceleration.y *= -1;
            this->setY(collider->up() - this->height());
            grounded = true;
        }

        //smackin into roof
        else if (collider->getCollider().intersects(collider->getCollider().getDown(), this->collider.getUp()))
        {
            this->momentum.y = 0;
            this->acceleration.y *= -1;
            this->setY(collider->down());
        }
    }

    else if (collider->getCollider().hasComponent(Collider::ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collider);
        if (throwable->getID() != 11) //is immune to birdbombs
            this->health.takeDamage(throwable->getDamage());
    }
}

void Bird::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0 && explosion.type != ExplosionType::bird)
    {
        int damage = explosion.calculateDamage(this->getCenterPos());
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
