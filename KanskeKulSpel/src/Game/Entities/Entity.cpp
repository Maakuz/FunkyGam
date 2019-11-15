#include "Entity.h"

Entity::Entity(sf::Vector2f pos, const sf::Texture * texture)
:collider(sf::Vector2f(0, 0), sf::Vector2f(0, 0)) 
{
    flipped = false;
    this->pos = pos;
    sprite.setPosition(pos);

    if (texture)
    {
        this->size = (sf::Vector2f)texture->getSize();
        sprite.setTexture(*texture, true);
        addCollision();
    }

    else
        addCollision(Collider::AABB(pos, sf::Vector2f(0, 0)));
}

void Entity::addCollision(Collider::AABB aabb)
{
    //test
    int diffX = Collider::EDGE_SIZE - aabb.size.x;
    int diffY = Collider::EDGE_SIZE - aabb.size.y;

    if (diffX > 0)
    {
        aabb.size.x += diffX;

        setSpriteOffset(sf::Vector2f(diffX / 2, this->spriteOffset.y));
    }

    if (diffY > 0)
    {
        aabb.size.y += diffY;

        setSpriteOffset(sf::Vector2f(this->spriteOffset.x, diffY / 2));
    }

    collider.setAABB(aabb);
    this->size = aabb.size;
    this->centerPos = this->pos + (this->size * 0.5f);
}

void Entity::addCollision()
{
    Collider::AABB aabb(this->pos, sf::Vector2f((float)sprite.getTextureRect().width, (float)sprite.getTextureRect().height));

    this->addCollision(aabb);
}

void Entity::flipHorizontally()
{
    sf::IntRect rect = sprite.getTextureRect();
    rect.width = -rect.width;
    sprite.setTextureRect(rect);
    flipped = !flipped;
}

void Entity::addCollisionComponent(Collider::ColliderKeys component)
{
    this->collider.addComponent(component);
}

void Entity::setSize(sf::Vector2f size)
{
    this->size = size;
    this->spriteOffset.y = -(abs(sprite.getTextureRect().height)-size.y);
    collider.setSize(size);
    this->centerPos = this->pos + (this->size * 0.5f);
}

void Entity::move(sf::Vector2f offset)
{
    setPosition(this->pos + offset);
}

void Entity::setPosition(sf::Vector2f pos)
{
    this->pos = pos;
    this->centerPos = this->pos + (this->size * 0.5f);
    updateSpritePosition();
}

void Entity::setX(float xPos)
{
    setPosition(sf::Vector2f(xPos, this->pos.y));
}

void Entity::setY(float yPos)
{
    setPosition(sf::Vector2f(this->pos.x, yPos));
}

void Entity::setSpriteOffset(sf::Vector2f offset)
{
    this->spriteOffset = offset;
}

void Entity::moveSpriteOffset(sf::Vector2f distance)
{
    this->spriteOffset += distance;
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
    this->collider.setPosition(pos);
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (sprite.getTexture() != nullptr)
        target.draw(this->sprite, states);
}
