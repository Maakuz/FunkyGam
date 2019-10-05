#include "Entity.h"

Entity::Entity(sf::Vector2f pos, sf::Texture * texture)
:collisionBox(sf::Vector2f(0, 0), sf::Vector2f(0, 0)) 
{
    flipped = false;
    this->pos = pos;
    this->size = (sf::Vector2f)texture->getSize();
    setPosition(pos);
    setTexture(*texture, true);
    addCollision();
}

void Entity::addCollision(CollisionBox::AABB aabb)
{
    collisionBox.setAABB(aabb);
    this->size = aabb.size;
}

void Entity::addCollision()
{
    CollisionBox::AABB aabb(getPosition(), sf::Vector2f((float)getTextureRect().width, (float)getTextureRect().height));

    this->addCollision(aabb);
}

void Entity::flipHorizontally()
{
    sf::IntRect rect = getTextureRect();
    rect.width = -rect.width;
    setTextureRect(rect);
    flipped = !flipped;
}

void Entity::updatePosition() 
{
    this->setPosition(pos); 
    this->collisionBox.setPosition(pos);
}
