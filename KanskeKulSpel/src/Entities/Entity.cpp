#include "Entity.h"

Entity::Entity(sf::Vector2f pos, int health, int speed, int attack)
{
    setPosition(pos);
}

void Entity::setCollisionBox(sf::IntRect collisionBox)
{
    this->collisionEnabled = true;
    this->collissionBox = collisionBox;
}

bool Entity::isCollisionEnabled() const
{
    return this->collisionEnabled;
}

void Entity::enableCollision(bool boi)
{
    this->collisionEnabled = boi;
}
