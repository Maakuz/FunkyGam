#include "Grunt.h"
#include "Misc/VectorFunctions.h"
#include "Misc/ConsoleWindow.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :Enemy(data, pos)
{
    this->roamDistance = 64;
    this->idleSpeed = 0.005;
    this->walkSpeed = idleSpeed;
    this->chaseSpeed = 0.01;
    this->state = State::idle;

    this->boundReached = 0;
    this->attackRange = 64;
    attackChargeTimer = Counter(1000);

    ConsoleWindow::get().addCommand("setState", [&](Arguments args)->std::string 
        {
            if (args.empty())
                return "Missing argument: 0 = idle, 1 = chasing, 2 = attack\n3 = returning, 4 = stunned";

            int val = std::stoi(args[0]);

            switch (val)
            {
            case 0:
                state = State::idle;
                break;

            case 1:
                state = State::chasing;
                break;

            case 2:
                state = State::attacking;
                break;

            case 3:
                state = State::returning;
                break;

            case 4:
                state = State::stunned;
                break;
            default:
                return "Not a valid argument: 0 = idle, 1 = chasing, 2 = attack\n3 = returning, 4 = stunned";
                break;
            }

            return "We did it!";
        });
}

void Grunt::update(float dt)
{
    
    switch (state)
    {
    case Enemy::State::idle:
        this->walkSpeed = idleSpeed;
        updateIdle(dt);
        break;

    case Enemy::State::chasing:
        this->walkSpeed = chaseSpeed;
        updateChasing(dt);
        break;

    case Enemy::State::attacking:
        updateAttack(dt);
        break;

    case Enemy::State::returning:
        updateReturning(dt);
        break;
    }

    Enemy::update(dt);
}


void Grunt::updateIdle(float dt)
{
    if (isDesicionTime() && this->boundReached == 0)
    {
        int r = rand() % 3;
        printf("%d\n", r);
        switch (r)
        {
        case 0:
            this->acceleration.x = 0;
            break;

        case 1:
            this->acceleration.x = -1;
            this->facingDir = Direction::left;
            if (!this->isFlippedHorizontally())
                this->flipHorizontally();
            break;

        case 2:
            this->acceleration.x = 1;
            if (this->isFlippedHorizontally())
                this->flipHorizontally();
            this->facingDir = Direction::right;
            break;

        default:
            this->acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    else if (isDesicionTime())
    {
        printf("End\n");
        if (this->facingDir == Direction::left)
            this->facingDir = Direction::right;

        else
            this->facingDir = Direction::left;

        this->acceleration.x = -boundReached;
        this->boundReached = 0;
        this->flipHorizontally();
        this->desicionTimeOver();
    }

    if (length(this->pos - this->getStartPoint()) > this->roamDistance && this->boundReached == 0)
    {
        this->boundReached = acceleration.x;
        this->acceleration.x = 0;
        this->momentum.x *= -1;
        this->desicionTimeOver();
    }

}

void Grunt::updateChasing(float dt)
{
    if (this->getLastKnownPos().x < this->pos.x)
    {
        this->facingDir = Direction::left;
        if (!this->isFlippedHorizontally())
            this->flipHorizontally();
        this->acceleration.x = -1;
    }

    else if (this->getLastKnownPos().x >= this->pos.x)
    {
        if (this->isFlippedHorizontally())
            this->flipHorizontally();
        this->facingDir = Direction::right;
        this->acceleration.x = 1;
    }


    if (lengthSquared(pos - getLastKnownPos()) < this->attackRange * this->attackRange)
    {
        state = State::attacking;
        this->attackChargeTimer.reset();
    }
}

void Grunt::updateReturning(float dt)
{
}

void Grunt::updateAttack(float dt)
{
    this->acceleration.x = 0;
    if (attackChargeTimer.update(dt))
    {
        if (facingDir == Direction::left)
            this->momentum = sf::Vector2f(-10, -5);

        else
            this->momentum = sf::Vector2f(10, -5);

        this->grounded = false;
        state = State::stunned;
    }
    printf("OmegaAttackj!\n");
}

void Grunt::handleCollision(const Entity& collider)
{
    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Platform))
    {
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.getPosition().y - this->size.y;
            grounded = true;
        }
    }

    if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Ground))
    {
        //walking on ground
        if (this->momentum.y > 0 && collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.getPosition().y - this->size.y;
            grounded = true;
        }

        //smackin into roof
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getDown(), this->collisionBox.getUp()))
        {
            this->momentum.y = 0;
            this->pos.y = collider.getPosition().y + collider.getSize().y;
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getLeft(), this->collisionBox.getRight()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.getPosition().x - this->size.x;
            this->jump();
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.getPosition().x + collider.getSize().x;
            this->jump();
        }
    }
}