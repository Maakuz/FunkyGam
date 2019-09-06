#include "Entity.h"

Entity::Entity(sf::Vector2f pos, sf::Texture * texture)
:collisionBox(sf::Vector2f(0, 0), sf::Vector2f(0, 0)) 
{
    setPosition(pos);
    setTexture(*texture, true);
    addCollision();
}

Entity::Entity(sf::Vector2f pos)
:collisionBox(sf::Vector2f(0, 0), sf::Vector2f(0, 0))
{
    setPosition(pos);
}

void Entity::addCollision(CollisionBox::AABB aabb)
{
    collisionBox.setBox(aabb);
}

void Entity::addCollision()
{
    //Change this to texture if this is not worinbg
    CollisionBox::AABB aabb(
        getPosition(), 
        sf::Vector2f(getTextureRect().width, getTextureRect().height) + getPosition());
}
