#include "Grunt.h"
#include "Misc/VectorFunctions.h"

Grunt::Grunt(AnimationData data, sf::Vector2f pos)
    :Enemy(data, pos)
{
    this->roamDecisionCounter = Counter(3000);
    this->roamDistance = 64;
    this->walkSpeed = 0.005;

}

void Grunt::update(float dt)
{
    if (isDesicionTime())
    {
        int r = rand() % 3;
        printf("%d\n", r);
        switch (r)
        {
        case 0:
            acceleration.x = 0;
            break;

        case 1:
            acceleration.x = -1;
            if (!this->isFlippedHorizontally())
                this->flipHorizontally();
            break;

        case 2:
            acceleration.x = 1;
            if (this->isFlippedHorizontally())
                this->flipHorizontally();
            break;

        default:
            acceleration.x = 0;
            break;
        }

        desicionTimeOver();
    }

    if (length(this->pos - this->getStartPoint()) > this->roamDistance)
    {
        acceleration.x = 0;
        momentum.x *= -1;
        desicionTimeOver();
    }

    Enemy::update(dt);
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

    MovingEntity::handleCollision(collider);
}