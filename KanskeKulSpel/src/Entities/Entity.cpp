#include "Entity.h"

Entity::Entity(sf::Vector2f pos, sf::Texture * texture)
:collisionBox(sf::Vector2f(0, 0), sf::Vector2f(0, 0)) 
{
    this->pos = pos;
    setPosition(pos);
    setTexture(*texture, true);
    addCollision();
}

void Entity::addCollision(CollisionBox::AABB aabb)
{
    collisionBox.setAABB(aabb);
}

void Entity::addCollision()
{
    //Change this to texture if this is not worinbg
    CollisionBox::AABB aabb(getPosition(), sf::Vector2f((float)getTextureRect().width, (float)getTextureRect().height));

    this->addCollision(aabb);
}
