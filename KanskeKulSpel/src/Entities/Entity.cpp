#include "Entity.h"

Entity::Entity(sf::Vector2f pos, sf::Texture * texture)
:collisionBox(sf::Vector2f(0, 0), sf::Vector2f(0, 0)) 
{
    flipped = false;
    this->pos = pos;
    this->size = (sf::Vector2f)texture->getSize();
    sprite.setPosition(pos);
    sprite.setTexture(*texture, true);
    addCollision();
}

void Entity::addCollision(CollisionBox::AABB aabb)
{
    collisionBox.setAABB(aabb);
    this->size = aabb.size;
}

void Entity::addCollision()
{
    CollisionBox::AABB aabb(this->pos, sf::Vector2f((float)sprite.getTextureRect().width, (float)sprite.getTextureRect().height));

    this->addCollision(aabb);
}

void Entity::flipHorizontally()
{
    sf::IntRect rect = sprite.getTextureRect();
    rect.width = -rect.width;
    sprite.setTextureRect(rect);
    flipped = !flipped;
}

void Entity::addCollisionComponent(CollisionBox::ColliderKeys component)
{
    this->collisionBox.addComponent(component);
}

void Entity::setSize(sf::Vector2f size)
{
    this->size = size;
    this->spriteOffset.y = -(abs(sprite.getTextureRect().height)-size.y);
    collisionBox.setSize(size);
}

void Entity::setPosition(sf::Vector2f pos)
{
    this->pos = pos;
    updateSpritePosition();
}

void Entity::setSpriteOffset(sf::Vector2f offset)
{
    this->spriteOffset = offset;
}

void Entity::moveSpriteOffset(sf::Vector2f distance)
{
    this->spriteOffset += distance;
}

sf::Vector2f Entity::getCenterPos() const
{
    sf::Vector2f center;
    center.x = this->pos.x + (this->size.x / 2);
    center.y = this->pos.y + (this->size.y / 2);
    return center;
}

float Entity::left() const
{
    return this->pos.x;
}

float Entity::right() const
{
    return this->pos.x + size.x;
}

float Entity::down() const
{
    return this->pos.y + size.y;
}

float Entity::up() const
{
    return this->pos.y;
}

float Entity::width() const
{
    return size.x;
}

float Entity::height() const
{
    return size.y;
}

void Entity::updateSpritePosition() 
{
    this->sprite.setPosition(pos + spriteOffset); 
    this->collisionBox.setPosition(pos);
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(this->sprite, states);
}
