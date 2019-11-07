#include "Bird.h"
#include "Game/Item/Throwables/Throwable.h"

Bird::Bird(AnimationData data, sf::Vector2f pos, UIHandler* ui)
    :Enemy(data, pos, ui)
{
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
        this->mass = 0;
        updateIdle(dt);
        break;

    case Enemy::State::chasing:
        setAnimation(0);
        this->walkSpeed = chaseSpeed;
        this->mass = 0;
        updateChasing(dt);
        break;

    case Enemy::State::attacking:
        setAnimation(1);
        updateAttack(dt);
        break;

    case Enemy::State::returning:
        setAnimation(0);
        this->mass = 0;
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

    Enemy::update(dt);
}

void Bird::handleCollision(const Entity* collider)
{
    if (collider->getCollider().hasComponent(Collider::ColliderKeys::ground) || collider->getCollider().hasComponent(Collider::ColliderKeys::platform))
    {
        //walking on ground
        if (this->momentum.y > 0 && collider->getCollider().intersects(collider->getCollider().getUp(), this->collider.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->up() - this->height();
            grounded = true;
        }

        //smackin into roof
        if (collider->getCollider().intersects(collider->getCollider().getDown(), this->collider.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider->down();
        }

        if (collider->getCollider().intersects(collider->getCollider().getLeft(), this->collider.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->left() - this->width();
            this->jump();
        }

        if (collider->getCollider().intersects(collider->getCollider().getRight(), this->collider.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider->right();
            this->jump();
        }
    }

    else if (collider->getCollider().hasComponent(Collider::ColliderKeys::throwable))
    {
        const Throwable* throwable = dynamic_cast<const Throwable*>(collider);
        this->health -= throwable->getDamage();
    }
}

void Bird::handleExplosion(const Explosion& explosion)
{
    if (explosion.damage > 0)
    {
        int damage = explosion.calculateDamage(this->getCenterPos());
        this->health -= damage;
        ui->displayEnemyDamage(float(health) / maxHealth);
    }
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
        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        this->acceleration = this->currentRoamPoint - this->pos;
        normalize(this->acceleration);

        this->desicionTimeOver();
    }

    sf::Vector2f roampointToPos = this->pos - this->currentRoamPoint;
    if (length(this->pos - this->currentRoamPoint) > this->roamDistance)
    {
        forcedDirection = Direction::right;
    }
}

void Bird::updateChasing(float dt)
{
}

void Bird::updateReturning(float dt)
{
}

void Bird::updateAttack(float dt)
{
}

std::istream& Bird::readSpecific(std::istream& in)
{
    return in;
}
