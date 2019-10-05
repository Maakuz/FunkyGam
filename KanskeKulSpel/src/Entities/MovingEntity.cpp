#include "MovingEntity.h"

MovingEntity::MovingEntity(AnimationData data, sf::Vector2f pos)
    :AnimatedEntity(data, pos)
{

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

    else if (collider.getCollisionBox().hasComponent(CollisionBox::colliderComponents::Platform))
    {
        //walking on ground
        if (collider.getCollisionBox().intersects(collider.getCollisionBox().getUp(), this->collisionBox.getDown()))
        {
            if (platformPassingCounter.isTimeUp())
            {
                this->momentum.y = 0;
                this->pos.y = collider.getPosition().y - this->size.y;
                grounded = true;
            }
        }
    }
}
