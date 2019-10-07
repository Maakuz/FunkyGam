#include "MovingEntity.h"
#include "Misc/Definitions.h"

MovingEntity::MovingEntity(AnimationData data, sf::Vector2f pos)
    :AnimatedEntity(data, pos)
{
    this->walkSpeed = 0.05f;
    this->floorRes = 0.85f;
    this->jumpHeight = 4.3f;
    this->mass = 0.166f;
    this->grounded = false;
}

void MovingEntity::update(float dt)
{
    momentum.x += acceleration.x * walkSpeed * dt;
    momentum.x *= floorRes;

    momentum.y += GRAVITY * dt * this->mass;
    momentum.y = std::min(TERMINALVELOCITY, momentum.y);

    this->pos += momentum;
    
    this->updatePosition();

    if (abs(momentum.x) < this->walkSpeed * 0.75f && !isIdle())
        pauseAnimation();

    else if (abs(momentum.x) > this->walkSpeed * 0.75f)
        resumeAnimation();

    this->updateAnimation(dt);
}


void MovingEntity::jump() // om släppa knapp trycka ner spelar lite jappjapp
{
    if (this->grounded)
    {
        momentum.y = -jumpHeight;
        this->grounded = false;
    }
}


void MovingEntity::handleCollision(const Entity& collider)
{
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
        }

        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getRight(), this->collisionBox.getLeft()))
        {
            this->momentum.x *= -0.5f;
            this->pos.x = collider.getPosition().x + collider.getSize().x;
        }
    }

    
}
