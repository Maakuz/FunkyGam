#include "Entity.h"

Entity::Entity(sf::Texture * texture, sf::Vector2i pos, int health, int speed, int attack)
    :Sprite(*texture)
{
    this->pos = pos;
}

Entity::Entity(sf::Vector2i pos, int health, int speed, int attack)
{
}

void Entity::setGridPos(sf::Vector2i pos)
{
    this->pos = pos;
}

sf::Vector2i Entity::getGridPos() const
{
    return this->pos;
}

void Entity::placeOnGrid(const Grid& grid)
{
    sf::Vector2f temp = grid.tileToWorldPos(this->pos);
    temp.y -= getTexture()->getSize().y - grid.getTileSize().y;
   setPosition(temp);
}
