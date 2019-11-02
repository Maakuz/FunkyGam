#include "Grunt.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"
#include <string>
#include "Entities/Items/Throwables/Throwable.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :Enemy(data, pos)
{
    this->roamDistance = 64;
    this->idleSpeed = 0.005f;
    this->walkSpeed = idleSpeed;
    this->chaseSpeed = 0.01f;
    this->state = State::idle;
    this->flying = false;
    this->forcedDirection = Direction::none;

    this->attackDistance = 64;
    this->attackChargeTimer = Counter(1000);
    this->stunCounter = Counter(500);
    this->collisionBox.addComponent(CollisionBox::ColliderKeys::grunt);
}

void Grunt::update(float dt)
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
        if (flying)
            setAnimation(2);

        else
            setAnimation(1);
        updateAttack(dt);
        break;

    case Enemy::State::searching:
        setAnimation(0);
        updateIdle(dt);
        if (searchCounter.update(dt))
        {
            this->currentRoamPoint = getStartPoint();
            this->state = State::returning;
        }
        break;

    case Enemy::State::returning:
        setAnimation(0);
        updateReturning(dt);
        break;

    case Enemy::State::stunned:
        setAnimation(3);
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
                this->searchCounter.reset();
            }
        }
        break;
    }

    Enemy::update(dt);
}


void Grunt::moveLeft()
{
    this->acceleration.x = -1;
    this->facingDir = Direction::left;
    if (!this->isFlippedHorizontally())
        this->flipHorizontally();
}

void Grunt::moveRight()
{
    this->acceleration.x = 1;
    if (this->isFlippedHorizontally())
        this->flipHorizontally();
    this->facingDir = Direction::right;
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
            this->acceleration.x = 0;
            break;

        case 1:
            moveLeft();
            break;

        case 2:
            moveRight();
            break;

        default:
            this->acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        printCon("End");
        if (this->forcedDirection == Direction::right)
            moveRight();

        else
            moveLeft();

        this->forcedDirection = Direction::none;

        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->pos - this->currentRoamPoint;
    if (length(this->pos - this->currentRoamPoint) > this->roamDistance)
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

    if (this->getLastKnownPos().x < this->pos.x)
    {
        moveLeft();
    }

    else if (this->getLastKnownPos().x >= this->pos.x)
    {
        moveRight();
    }


    if (lengthSquared(pos + eyeLevel - getLastKnownPos()) < this->attackDistance * this->attackDistance)
    {
        if (timeSincePlayerSeen < 200)
        {
            state = State::attacking;
            this->attackChargeTimer.reset();
        }
    }

    if(timeSincePlayerSeen > 200 && abs(pos.x - getLastKnownPos().x) < 10)
    {
        drawQuestion.reset();
        state = State::searching;
        this->currentRoamPoint = getLastKnownPos();
        this->searchCounter.reset();
    }
}

void Grunt::updateReturning(float dt)
{
    if (this->currentRoamPoint.x < this->pos.x)
    {
        moveLeft();
    }

    else if (this->currentRoamPoint.x >= this->pos.x)
    {
        moveRight();
    }

    if (abs(pos.x - currentRoamPoint.x) < 10)
    {
        state = State::idle;
    }
}

void Grunt::updateAttack(float dt)
{
    this->acceleration.x = 0;
    if (attackChargeTimer.update(dt) && !flying)
    {
        if (facingDir == Direction::left)
            this->momentum = sf::Vector2f(-attackMomentum.x, -attackMomentum.y);

        else
            this->momentum = sf::Vector2f(attackMomentum.x, -attackMomentum.y);

        this->grounded = false;
        this->flying = true;
    }
}

std::istream& Grunt::readSpecific(std::istream& in)
{
    std::string trash;
    in >> trash;
    in >> trash >> chaseSpeed;
    in >> trash >> idleSpeed;
    in >> trash >> jumpHeight;
    in >> trash >> attackMomentum.x >> attackMomentum.y;
    in >> trash >> attackDistance;
    in >> trash >> damage;
    in >> trash >> attackChargeTimer.stopValue;
    in >> trash >> stunCounter.stopValue;
    in >> trash >> searchCounter.stopValue;
    return in;
}

void Grunt::handleCollision(const Entity* collider)
{
    if (collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::ground) || collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::platform))
    {
        //walking on ground
        if (this->momentum.y > 0 && collider->getCollisionBox().intersects(collider->getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->up() - this->height();
            grounded = true;

            if (flying)
            {
                state = State::chasing;
                flying = false;
            }
        }

        //smackin into roof
        if (collider->getCollisionBox().intersects(collider->getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->down();
        }

        if (collider->getCollisionBox().intersects(collider->getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->left() - this->width();
            this->jump();
        }

        if (collider->getCollisionBox().intersects(collider->getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->right();
            this->jump();
        }
    }

    else if (flying && !collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::player))
    {
        this->flying = false;
        this->state = State::stunned;
    }

    else if (flying && collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::player))
    {
        const MovingEntity* ptr = dynamic_cast<const MovingEntity*>(collider);
        
        this->addCollisionMomentum(ptr->getMomentum(), ptr->getCenterPos(), ptr->getMass());
    }

    if (collider->getCollisionBox().hasComponent(CollisionBox::ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collider);
        this->health -= throwable->getDamage();

        printCon(std::to_string(this->health));
    }
}

void Grunt::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(this->getCenterPos());
        this->health -= damage;
    }

    switch (explosion.type)
    {
    case ExplosionType::flash:
        state = State::stunned;
        stunCounter.reset();
        break;

    case ExplosionType::sound:
        if (state != State::stunned && state != State::chasing && state != State::attacking)
        {
            this->currentRoamPoint = explosion.center;
            this->state = State::searching;
            this->drawQuestion.reset();
            searchCounter.reset();

            if (explosion.center.x < this->pos.x)
                this->moveLeft();

            else
                moveRight();
        }
        break;
    default:
        break;
    }
}
