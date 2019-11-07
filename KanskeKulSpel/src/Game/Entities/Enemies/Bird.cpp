#include "Bird.h"

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
            }
        }
        break;
    }

    Enemy::update(dt);
}

void Bird::handleCollision(const Entity* collider)
{
}

void Bird::handleExplosion(const Explosion& explosion)
{
}

void Bird::updateIdle(float dt)
{
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
