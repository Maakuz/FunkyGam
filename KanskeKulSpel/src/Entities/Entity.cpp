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

sf::Vector2f Entity::getCenterPos() const
{
    sf::Vector2f center;
    center.x = this->pos.x + (this->size.x / 2);
    center.y = this->pos.y + (this->size.y / 2);
    return center;
}

void Entity::updatePosition() 
{
    this->setPosition(pos); 
    this->collisionBox.setPosition(pos);
}
